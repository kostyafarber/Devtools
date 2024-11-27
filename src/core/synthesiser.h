
#include "ru/clamped.h"
#include <cstdint>
namespace core {
class Synthesiser
{
  enum class WaveType
  {
    Square,
  };

public:
  Synthesiser(uint32_t sample_rate, uint32_t frequency, float duty_cycle)
      : m_sample_rate(sample_rate), m_frequency(frequency),
        m_duty_cycle(duty_cycle),
        m_phase_increment(m_frequency / m_sample_rate),
        m_clamper(0.0f, 1.0f) {};
  ~Synthesiser() = default;

  Synthesiser(const Synthesiser &) = delete;
  const Synthesiser &operator=(const Synthesiser &) = delete;

  Synthesiser(Synthesiser &&) = default;
  Synthesiser &operator=(Synthesiser &&) = default;

  void increase_volume(float increase) noexcept;
  void decrease_volume(float decrease) noexcept;
  float volume() const { return m_volume; }

  float generate() noexcept;

  void set_frequency(float frequency) noexcept;
  void set_duty_cycle(float duty_cycle) noexcept;

private:
  std::atomic<float> m_frequency;
  float m_sample_rate;             // samples per second
  std::atomic<float> m_duty_cycle; // ratio of high (1) to low (-1)
  float m_phase{0};                // current position in wave cycle
  float m_phase_increment; // how much to advance phase per sample (was m_cycle)
  std::atomic<float> m_volume{1.0f}; // amplitude (controls volume)

  Clamped<float> m_clamper;
};
} // namespace core