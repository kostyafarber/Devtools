#include "base/error.h"
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>

namespace core
{

static OSStatus audio_callback(void *inRefCon,
                               AudioUnitRenderActionFlags *ioActionFlags,
                               const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber, UInt32 inNumberFrames,
                               AudioBufferList *ioData);

class AudioProcess
{
public:
  std::string m_name;

  AudioProcess(const std::string &name) : m_name(name) {};
  ~AudioProcess() = default;

  AudioProcess(const AudioProcess &) = delete;
  const AudioProcess &operator=(const AudioProcess &) = delete;

  AudioProcess(AudioProcess &&) = default;
  AudioProcess &operator=(AudioProcess &&) = default;

  base::ErrorOr<void> start() { return {}; };
  base::ErrorOr<void> play() { return {}; };
  base::ErrorOr<void> pause() { return {}; };
  base::ErrorOr<void> stop() { return {}; };

  base::ErrorOr<void> initialise();
  base::ErrorOr<void> shutdown();

private:
  bool m_initialised{false};
  AudioUnit m_audio_unit{};
  AudioComponentInstance m_component{};

  base::ErrorOr<void> setup_audio_buffers();
  base::ErrorOr<void> process_audio_buffers();
};
} // namespace core