#include "base/error.h"
#include "synthesiser.h"
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <memory>

namespace core {

static OSStatus audio_callback(void *inRefCon,
                               AudioUnitRenderActionFlags *ioActionFlags,
                               const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber, UInt32 inNumberFrames,
                               AudioBufferList *ioData);

class AudioProcess
{
public:
  std::string m_name;
  std::unique_ptr<core::Synthesiser> m_synth;

  AudioProcess(const std::string &name)
      : m_name(name),
        m_synth(std::make_unique<Synthesiser>(Synthesiser(44100, 440, 0.5))) {};
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