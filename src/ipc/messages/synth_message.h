#include "synth_command.h"
#include <cstdint>

namespace ipc {

class SynthMessage
{
  SynthCommand m_message;

  union PayloadData {
    struct {
      float frequency;
    } freq;
    
    std::uint8_t padding[16];
  } data;
};
} // namespace ipc