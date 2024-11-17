#include "ring_buffer.h"

namespace ru
{

base::ErrorOr<float> RingBuffer::read(size_t pos)
{
  if (pos > m_size)
    return base::Error::from_string("position can't be greater than buffer");

  if (m_samples.empty())
    return base::Error::from_string("samples are empty");

  return m_samples[m_size % pos];
}

} // namespace ru