#pragma once

#include <cerrno>
#include <cstring>
#include <string>
#include <variant>

namespace base {
class Error
{
public:
  // better to specify this as the context rather than a message
  static Error from_string(const std::string &context)
  {
    return Error(context);
  }

  // need to make it static
  static Error from_errno(const std::string &context)
  {
    // how to assign (make it moveable)
    // to avoid copying I've made this I'll make this a static method
    auto sys_error = std::strerror(errno);

    return Error(context + ":" + sys_error);
  };

  std::string message() { return m_message; }

private:
  std::string m_message;

  explicit Error(std::string msg) : m_message(msg){};
};
struct Empty {
};

template <typename R, typename E = Error>
class ErrorOr
{
private:
  std::variant<R, E> m_value;

public:
  // two constructors for each type
  ErrorOr(R value) : m_value(std::move(value)){};
  ErrorOr(E value) : m_value(std::move(value)){};

  bool is_error() { return std::holds_alternative<E>(m_value); }

  R &value() { return std::get<R>(m_value); }
  const R &value() const { return std::get<R>(m_value); }

  E &error() { return std::get<E>(m_value); }
  const E &error() const { return std::get<E>(m_value); }
};

template <typename E>
class ErrorOr<void, E> : public ErrorOr<Empty, E>
{
public:
  using ErrorOr<Empty, E>::ErrorOr;

  ErrorOr() : ErrorOr<Empty, E>(Empty{}){};
};

} // namespace base