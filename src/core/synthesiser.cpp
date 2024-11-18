#include "synthesiser.h"

void core::Synthesiser::set_frequency(float frequency) noexcept
{
  m_frequency = frequency;
  m_phase_increment = m_frequency / m_sample_rate;
}

void core::Synthesiser::set_duty_cycle(float duty_cycle) noexcept
{
  m_duty_cycle = m_clamper.value(duty_cycle);
}

void core::Synthesiser::increase_volume(float increase) noexcept
{
  m_volume = m_clamper.value(increase);
}

void core::Synthesiser::decrease_volume(float increase) noexcept
{
  m_volume = m_clamper.value(increase);
}

float core::Synthesiser::generate() noexcept
{
  m_phase += m_phase_increment;

  if (m_phase >= 1.0f)
    m_phase -= 1.0f;

  auto sample = (m_phase > m_duty_cycle) ? 1.0f : -1.0f;
  return sample * m_volume;
};
