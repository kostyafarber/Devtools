#pragma once

#include "error.h"
#include <cstddef>
#include <cstdint>
#include <span>

namespace ipc {

class MessageHeader
{
public:
  MessageHeader() : m_header_raw(Raw{.magic = MAGIC, .size = 0}) {};

  base::ErrorOr<void> validate() noexcept
  {
    if (m_header_raw.magic != MAGIC) {
      return base::Error::from_string("invalid magic");
    }

    if (m_header_raw.size > MAX_MESSAGE_SIZE) {
      return base::Error::from_string("size it too large");
    }

    return {};
  }

  std::span<std::byte> data()
  {
    return std::span<std::byte>(reinterpret_cast<std::byte *>(&m_header_raw),
                                WIRE_SIZE);
  };

  void set_size(size_t size) { m_header_raw.size = size; }
  size_t payload_size() const { return m_header_raw.size; }

private:
  static constexpr uint32_t MAGIC = 0x53594E48;
  static constexpr size_t MAX_MESSAGE_SIZE = 1024;
  static constexpr size_t WIRE_SIZE = sizeof(uint32_t) * 2;

  struct Raw {
    uint32_t magic;
    uint32_t size;
  };

  Raw m_header_raw;
};

} // namespace ipc