#include "logging.h"

namespace base {
void log(base::LogLevel, const std::string &msg)
{
  // switch on the log level and colour it? and maybe check if we have log level
  // defined but where at the compile time or component level let's just return
  // it
  //
  std::cout << msg << std::endl;
}
} // namespace base