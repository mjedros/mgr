#include "Logger.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

namespace Mgr {
void Logger::printError(cl::Error &err) {
  std::cout << err.what() << std::endl;
  std::cout << err.err() << std::endl;
}

void Logger::printText(std::string text) { std::cout << text << std::endl; }

Logger::Logger() {}
}
