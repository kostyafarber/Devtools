#include "synthesiser.h"
#include <algorithm>

void core::Synthesiser::set_frequency(float frequency)
{
  m_frequency = frequency;
  m_phase_increment = m_sample_rate / frequency;
}

void core::Synthesiser::set_duty_cycle(float duty_cycle)
{
  m_duty_cycle = std::clamp(duty_cycle, 0.0f, 1.0f);
}

float core::Synthesiser::generate() noexcept
{
  m_phase += m_phase_increment;

  if (m_phase >= 1.0f)
    m_phase -= 1.0f;

  auto sample = (m_phase > m_duty_cycle) ? 1.0f : -1.0f;
  return sample * m_volume;
};
