#include "synthesiser.h"
#include <atomic>

void core::Synthesiser::set_frequency(float frequency) noexcept
{
  m_frequency.store(frequency, std::memory_order_release);
  m_phase_increment = m_frequency / m_sample_rate;
}

void core::Synthesiser::set_duty_cycle(float duty_cycle) noexcept
{
  m_duty_cycle.store(m_clamper.value(duty_cycle), std::memory_order_release);
}

void core::Synthesiser::increase_volume(float increase) noexcept
{
  m_volume.store(m_clamper.value(m_volume + increase),
                 std::memory_order_release);
}

void core::Synthesiser::decrease_volume(float decrease) noexcept
{
  m_volume.store(m_clamper.value(m_volume - decrease),
                 std::memory_order_release);
}

float core::Synthesiser::generate() noexcept
{
  m_phase += m_phase_increment;

  if (m_phase >= 1.0f)
    m_phase -= 1.0f;

  auto sample = (m_phase > m_duty_cycle) ? 1.0f : -1.0f;
  return sample * m_volume.load(std::memory_order_acquire);
};
