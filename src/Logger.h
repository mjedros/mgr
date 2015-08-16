#pragma once

#include <chrono>
#include <string>
#include <fstream>
#include <vector>
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
  void endOperation(const unsigned long &clDuration = 0);
  void resetTimer();
  void beginOperation();
  void printAvarageTime();
  void startReadingImage();
  void stopReadingImage();
  void printFancyLine(std::string line);
  void printProcessing(const std::string &operationString,
                       const std::string &MorphElementType,
                       const std::vector<float> StructElemParams,
                       const std::string &operationWay,
                       bool processROI = false);

private:
  std::fstream file;
  unsigned int operationsCounter = 1;
  unsigned int timeSum = 0;
  unsigned int transferSum = 0;
  std::chrono::time_point<std::chrono::system_clock> allSum;
  unsigned long int clSum = 0;
  std::chrono::time_point<std::chrono::system_clock> start;
  std::chrono::time_point<std::chrono::system_clock> startReading;
  Logger();
  Logger(const Logger &logger) = delete;
  ~Logger();
};
}
