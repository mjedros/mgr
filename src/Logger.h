#pragma once

#include <string>
#include <iostream>

namespace cl {
class Error;
}
namespace Mgr {
class Logger {
public:
  static Logger &getInstance() {
    static Logger logger;
    return logger;
  }
  void printError(cl::Error &err);
  void printText(std::string text);

private:
  Logger();
  Logger(const Logger &logger) = delete;
};
}
