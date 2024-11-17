#include "audio.h"
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudioTypes/CoreAudioTypes.h>
#include <MacTypes.h>
#include <math.h>

namespace core
{
static OSStatus audio_callback(void *inRefCon,
                               AudioUnitRenderActionFlags *ioActionFlags,
                               const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber, UInt32 inNumberFrames,
                               AudioBufferList *ioData)
{
  // Cast the reference back to AudioProcess
  auto *audio_process = static_cast<AudioProcess *>(inRefCon);

  // Generate a 440Hz tone (A4 note)
  static double phase = 0.0;
  const double frequency = 440.0; // A4 note
  const double amplitude = 0.5;   // Half volume

  Float32 *buffer = static_cast<Float32 *>(ioData->mBuffers[0].mData);
  for (UInt32 i = 0; i < inNumberFrames; ++i)
  {
    buffer[i] = amplitude * sin(phase);
    phase += 2.0 * M_PI * frequency / 44100.0;
    if (phase >= 2.0 * M_PI)
      phase -= 2.0 * M_PI;
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
  format.mSampleRate = 44100;
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

  // Initialize the unit
  status = AudioUnitInitialize(m_audio_unit);
  if (status != noErr)
  {
    return base::Error::from_string("Failed to initialize audio unit");
  }

  // Start it playing
  status = AudioOutputUnitStart(m_audio_unit);
  if (status != noErr)
  {
    return base::Error::from_string("Failed to start audio unit");
  }

  m_initialised = true;

  return {};
}

} // namespace core
// Add this callback function declaration before initialise()
