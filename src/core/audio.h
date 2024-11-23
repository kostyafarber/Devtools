#include "ru/ring_buffer.h"
#include "synthesiser.h"
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <memory>
#include <sys/_types/_u_int32_t.h>
#include <thread>
#include <vector>

namespace core {

struct AudioConfig {
  u_int32_t frequency;
  u_int32_t sampling_rate;
  u_int32_t buffer_size = 1024;
};

class AudioProcess
{
public:
  std::string m_name;
  std::unique_ptr<core::Synthesiser> m_synth;

  AudioProcess(const std::string &name, AudioConfig audio_config)
      : m_name(name), m_audio_config(audio_config),
        m_buffer(audio_config.buffer_size * 2),
        m_temp_buffer(audio_config.buffer_size){};
  ~AudioProcess() = default;

  AudioProcess(const AudioProcess &) = delete;
  const AudioProcess &operator=(const AudioProcess &) = delete;

  AudioProcess(AudioProcess &&) = default;
  AudioProcess &operator=(AudioProcess &&) = default;

  float get_fill_percentage() const;
  u_int32_t get_underrun_count() const;
  u_int32_t get_overflow_count() const;

  base::ErrorOr<void> play();
  base::ErrorOr<void> pause() { return {}; };
  base::ErrorOr<void> stop();

  base::ErrorOr<void> initialise();
  base::ErrorOr<void> shutdown();

private:
  struct BufferStats {
    float fill_percentage;
    u_int32_t underrun_count;
    u_int32_t overflow_count;
  };

  AudioConfig m_audio_config;

  bool m_initialised{false};
  bool m_playing{false};
  void write_samples() noexcept;

  AudioUnit m_audio_unit{};
  AudioComponentInstance m_component{};

  ru::RingBuffer<float> m_buffer{};
  std::vector<float> m_temp_buffer{};

  std::thread m_writer_thread;

  friend OSStatus audio_callback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber, UInt32 inNumberFrames,
                                 AudioBufferList *ioData);
};

} // namespace core