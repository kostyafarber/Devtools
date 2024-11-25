#include "logging.h"
#include <iostream>

namespace base {
void log(base::LogLevel level, const std::string &category,
         const std::string &msg)
{
  std::cout << "[" << category << "] " << msg << std::endl;
}
} // namespace base