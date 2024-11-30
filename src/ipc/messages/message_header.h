#pragma once
#include <cstddef>
#include <cstdint>

namespace ipc {

class MessageHeader
{
public:
  struct Raw {
    uint32_t magic;
    uint32_t size;
  };

  enum class ValidationResult
  {
    Valid,
    InvalidMagic,
    SizeTooLarge
  };

  static ValidationResult validate(const Raw &raw) noexcept
  {
    if (raw.magic != MAGIC)
      return ValidationResult::InvalidMagic;

    if (raw.size > MAX_MESSAGE_SIZE)
      return ValidationResult::SizeTooLarge;

    return ValidationResult::Valid;
  }

  static constexpr uint32_t MAGIC = 0x53594E48;
  static constexpr size_t MAX_MESSAGE_SIZE = 1024;
};

} // namespace ipc