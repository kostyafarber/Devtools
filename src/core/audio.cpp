#include "audio.h"
#include "base/error.h"
#include "ipc/messages/synth_message.h"
#include "ipc/socket_server.h"
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudioTypes/CoreAudioTypes.h>
#include <MacTypes.h>
#include <algorithm>
#include <atomic>
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
  if (!(audio_process->m_callback_buffer.read(buffer, inNumberFrames))) {
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

  auto maybe_socket_server =
      ipc::SocketServer::create("tmp/command.socket", m_command_buffer);
  if (maybe_socket_server.is_error())
    return maybe_socket_server.error();

  m_socket_server = std::move(maybe_socket_server.value());
  m_socket_server->start();

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

  m_playing.store(true, std::memory_order_release);
  m_callback_thread = std::thread([this]() { write_samples(); });

  return {};
}

base::ErrorOr<void> AudioProcess::stop()
{

  m_playing.store(false, std::memory_order_release);

  if (m_callback_thread.joinable()) {
    m_callback_thread.join();
  }

  auto status = AudioOutputUnitStop(m_audio_unit);
  if (status != noErr) {
    return base::Error::from_string("Failed to start audio unit");
  }

  return {};
}

void AudioProcess::process_commands(const ipc::SynthMessage &message) noexcept
{
  switch (message.m_message) {
  case ipc::SynthCommand::IncreaseVolume:
    LOG_AUDIO(Info, "increasing volume by: {:2f}", message.data.volume);
    m_synth->increase_volume(message.data.volume);
    break;

  case ipc::SynthCommand::DecreaseVolume:
    LOG_AUDIO(Info, "decreeing volume by: {:2f}", message.data.volume);
    m_synth->decrease_volume(message.data.volume);
    break;

  case ipc::SynthCommand::SetDutyCycle:
    LOG_AUDIO(Info, "setting duty cycle to: {:2f}", message.data.duty_cycle);
    m_synth->set_duty_cycle(message.data.duty_cycle);
    break;

  case ipc::SynthCommand::SetFrequency:
    LOG_AUDIO(Info, "setting frequency to: {:2f}", message.data.frequency);
    m_synth->set_frequency(message.data.frequency);
    break;

  case ipc::SynthCommand::Start:
    LOG_AUDIO(Info, "starting playback");
    play();
    break;

  case ipc::SynthCommand::Stop:
    LOG_AUDIO(Info, "stopping playback");
    stop();
    break;
  }
}

void AudioProcess::write_samples() noexcept
{
  const float buffer_duration_ms =
      static_cast<float>(m_audio_config.buffer_size) /
      m_audio_config.sampling_rate * 1000;
  const auto sleep_duration =
      std::chrono::milliseconds(static_cast<int>(buffer_duration_ms / 2));

  while (m_initialised && m_playing.load(std::memory_order_acquire)) {
    ipc::SynthMessage msg;
    if (m_command_buffer.read(&msg, sizeof(msg))) {
      LOG_AUDIO(Info, "processing command");
      process_commands(msg);
    }

    for (int i = 0; i < m_audio_config.buffer_size; i++) {
      m_temp_buffer[i] = m_synth->generate();
    }

    if (!(m_callback_buffer.write(&m_temp_buffer[0],
                                  m_audio_config.buffer_size)))
      LOG_AUDIO(Warn, "buffer is full");

    std::this_thread::sleep_for(sleep_duration);
  }
}

float AudioProcess::get_fill_percentage() const { return 0.0f; }

} // namespace core
  // Add this callback function declaration before initialise()
