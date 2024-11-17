
namespace core {
class Synthesiser
{
  enum class WaveType
  {
    Square,
  };

  float m_frequency;
  const float m_sample_rate; // samples per second
  float m_duty_cycle;        // ratio of high (1) to low (-1)
  float m_phase{0};          // current position in wave cycle
  float m_phase_increment; // how much to advance phase per sample (was m_cycle)
  float m_volume{1.0f};    // amplitude (controls volume)

public:
  Synthesiser(int sample_rate, int frequency)
      : m_sample_rate(sample_rate), m_frequency(frequency),
        m_phase_increment(m_sample_rate / m_frequency) {};

  bool increase_volume();
  bool decrease_volume();
  float generate() noexcept;

  void set_frequency(float frequency);
  void set_duty_cycle(float duty_cycle);
};
} // namespace core