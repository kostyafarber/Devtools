#include "ipc/messages/synth_message.h"
#include "ipc/socket_server.h"
#include "ru/ring_buffer.h"
#include "synthesiser.h"
#include <AudioToolbox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <atomic>
#include <memory>
#include <sys/_types/_u_int32_t.h>
#include <thread>
#include <vector>

namespace core {

struct AudioConfig {
  u_int32_t frequency;
  u_int32_t sampling_rate;
  u_int32_t buffer_size = 1024;
  static constexpr u_int32_t command_buffer_size = 128;
};

class AudioProcess
{
public:
  std::string m_name;
  std::unique_ptr<core::Synthesiser> m_synth;

  AudioProcess(const std::string &name, AudioConfig audio_config)
      : m_name(name), m_audio_config(audio_config),
        m_callback_buffer(audio_config.buffer_size * 2),
        m_command_buffer(audio_config.command_buffer_size),
        m_temp_buffer(audio_config.buffer_size),
        m_synth(std::make_unique<core::Synthesiser>(
            audio_config.sampling_rate, audio_config.frequency, 0.5)){};
  ~AudioProcess()
  {
    m_initialised.store(false, std::memory_order_release);
    if (m_playing) {
      stop();
    }

    if (m_command_thread.joinable())
      m_command_thread.join();
  }

  AudioProcess(const AudioProcess &) = delete;
  const AudioProcess &operator=(const AudioProcess &) = delete;

  float get_fill_percentage() const;
  u_int32_t get_underrun_count() const;
  u_int32_t get_overflow_count() const;

  base::ErrorOr<void> play();
  base::ErrorOr<void> pause() { return {}; };
  base::ErrorOr<void> stop();

  void increase_volume(float increase) { m_synth->increase_volume(increase); }
  void decrease_volume(float decrease) { m_synth->decrease_volume(decrease); }

  base::ErrorOr<void> initialise();
  base::ErrorOr<void> shutdown();

private:
  struct BufferStats {
    float fill_percentage;
    u_int32_t underrun_count;
    u_int32_t overflow_count;
  };

  AudioConfig m_audio_config;

  std::atomic<bool> m_initialised{false};

  std::atomic<bool> m_playing{false};
  std::atomic<bool> m_listening_for_commands{false};

  void write_samples() noexcept;
  void process_command(const ipc::SynthMessage &message) noexcept;
  void read_commands() noexcept;

  AudioUnit m_audio_unit{};
  AudioComponentInstance m_component{};

  ru::RingBuffer<float> m_callback_buffer{};
  std::vector<float> m_temp_buffer{};

  ru::RingBuffer<ipc::SynthMessage> m_command_buffer{};
  std::optional<ipc::SocketServer> m_socket_server;

  std::thread m_callback_thread;
  std::thread m_command_thread;

  friend OSStatus audio_callback(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber, UInt32 inNumberFrames,
                                 AudioBufferList *ioData);
};

} // namespace core