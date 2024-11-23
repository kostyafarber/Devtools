#include <format>
#include <iostream>
#include <string>

namespace base {
enum class LogLevel
{
  Debug,
  Info,
  Warn,
  Error,
};

void log(LogLevel level, const std::string &category, const std::string &msg);

#ifdef ENABLE_AUDIO_DEBUG
#define LOG_AUDIO(level, msg) base::log(base::LogLevel::level, "Audio", msg)
#else
#define LOG_AUDIO(level, msg)
#endif

} // namespace base
