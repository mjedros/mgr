#pragma once

#include <chrono>
#include <string>
#include <fstream>
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
  void printLine(std::string line);
  void printProcessingROI(bool roi);
  void endOperation();
  void resetTimer();
  void beginOperation();
  void printAvarageTime();

private:
  std::fstream file;
  unsigned int operationsCounter;
  unsigned int timeSum;
  std::chrono::time_point<std::chrono::system_clock> start;

  Logger();
  Logger(const Logger &logger) = delete;
  ~Logger();
};
}
