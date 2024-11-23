#include "audio.h"
#include "base/logging.h"
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudioTypes/CoreAudioTypes.h>
#include <MacTypes.h>
#include <algorithm>
#include <thread>

namespace core {

OSStatus audio_callback(void *inRefCon,
                        AudioUnitRenderActionFlags *ioActionFlags,
                        const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                        UInt32 inNumberFrames, AudioBufferList *ioData)
{
  // Cast the reference back to AudioProcess
  auto *audio_process = static_cast<AudioProcess *>(inRefCon);

  LOG_AUDIO(Debug, "calling audio_callback");
  Float32 *buffer = static_cast<Float32 *>(ioData->mBuffers[0].mData);
  if (!(audio_process->m_buffer.read(buffer, inNumberFrames))) {
    LOG_AUDIO(Warn, "buffer underrun");
    std::fill_n(buffer, inNumberFrames, 0.0f);
  }

  return noErr;
}

base::ErrorOr<void> AudioProcess::initialise()
{
  AudioComponentDescription desc{};

  desc.componentType = kAudioUnitType_Output;
  desc.componentSubType = kAudioUnitSubType_DefaultOutput;
  desc.componentManufacturer = kAudioUnitManufacturer_Apple;

  AudioComponent component = AudioComponentFindNext(nullptr, &desc);

  if (!component)
    return base::Error::from_string("error finding audio component");

  OSStatus status = AudioComponentInstanceNew(component, &m_audio_unit);
  if (status != noErr)
    return base::Error::from_string("error creating audio component instance");

  AudioStreamBasicDescription format{};
  format.mSampleRate = m_audio_config.sampling_rate;
  format.mFormatID = kAudioFormatLinearPCM;
  format.mChannelsPerFrame = 1;
  format.mBitsPerChannel = 32;

  format.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
  format.mFramesPerPacket = 1;
  format.mBytesPerFrame = sizeof(Float32);
  format.mBytesPerPacket = sizeof(Float32);

  status =
      AudioUnitSetProperty(m_audio_unit, kAudioUnitProperty_StreamFormat,
                           kAudioUnitScope_Input, 0, &format, sizeof(format));

  if (status != noErr)
    return base::Error::from_string("error setting audio properties");

  AURenderCallbackStruct callback_struct;
  callback_struct.inputProc = audio_callback;
  callback_struct.inputProcRefCon = this; // Pass our instance

  status = AudioUnitSetProperty(
      m_audio_unit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input,
      0, &callback_struct, sizeof(callback_struct));

  status = AudioUnitInitialize(m_audio_unit);
  if (status != noErr) {
    return base::Error::from_string("Failed to initialize audio unit");
  }

  m_initialised = true;

  return {};
}

base::ErrorOr<void> AudioProcess::play()
{
  if ((!m_initialised))
    return base::Error::from_string("audio unit not initialised");

  auto status = AudioOutputUnitStart(m_audio_unit);
  if (status != noErr) {
    return base::Error::from_string("Failed to start audio unit");
  }

  m_playing = true;
  m_writer_thread = std::thread([this]() { write_samples(); });

  return {};
}

base::ErrorOr<void> AudioProcess::stop()
{

  m_playing = false;

  if (m_writer_thread.joinable()) {
    m_writer_thread.join();
  }

  auto status = AudioOutputUnitStop(m_audio_unit);
  if (status != noErr) {
    return base::Error::from_string("Failed to start audio unit");
  }

  return {};
}

void AudioProcess::write_samples() noexcept
{
  const float buffer_duration_ms =
      static_cast<float>(m_audio_config.buffer_size) /
      m_audio_config.sampling_rate * 1000;
  const auto sleep_duration =
      std::chrono::milliseconds(static_cast<int>(buffer_duration_ms / 2));

  while (m_initialised && m_playing) {
    for (int i = 0; i < m_audio_config.buffer_size; i++) {
      m_temp_buffer[i] = m_synth->generate();
    }

    if (!(m_buffer.write(&m_temp_buffer[0], m_audio_config.buffer_size))) {
      LOG_AUDIO(Warn, "buffer is full");
    }

    std::this_thread::sleep_for(sleep_duration);
  }
}

float AudioProcess::get_fill_percentage() const {return 0.0f;}

} // namespace core
// Add this callback function declaration before initialise()
