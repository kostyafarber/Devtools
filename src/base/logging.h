#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

namespace base {
enum class LogLevel
{
  Debug,
  Info,
  Warn,
  Error,
};

class Logger
{
public:
  template <typename... Args>
  static void log(LogLevel level, const std::string &category,
                  std::format_string<Args...> fmt, Args &&...args)
  {

    auto get_level = [](LogLevel level) {
      switch (level) {
      case base::LogLevel::Error:
        return std::string("Error");
      case base::LogLevel::Debug:
        return std::string("Debug");
      case base::LogLevel::Info:
        return std::string("Info");
      case base::LogLevel::Warn:
        return std::string("Warn");
      }

      return std::string("Unk");
    };

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto *tm = std::localtime(&time);

    std::stringstream ss;
    ss << std::format("{:02d}:{:02d}:{:02d} ", tm->tm_hour, tm->tm_min,
                      tm->tm_sec)
       << get_level(level) << " [" << category << "] "
       << std::format(fmt, std::forward<Args>(args)...) << std::endl;

    std::cout << ss.str();
  }
};

#ifdef ENABLE_AUDIO_DEBUG
#define LOG_AUDIO(level, fmt, ...)                                             \
  base::Logger::log(base::LogLevel::level, "Audio",                            \
                    fmt __VA_OPT__(, ) __VA_ARGS__)
#else
#define LOG_AUDIO(level, fmt, ...)
#endif
} // namespace base
