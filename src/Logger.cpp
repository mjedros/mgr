#include "Logger.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

namespace Mgr {
void Logger::printError(cl::Error &err) {
  std::cout << err.what() << std::endl;
  std::cout << err.err() << std::endl;
}

void Logger::printText(std::string text) { std::cout << text << std::endl; }

void Logger::endOperation() {
  auto end = std::chrono::system_clock::now();
  timeSum += std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                 .count();
}

void Logger::resetTimer() {
  operationsCounter = 0;
  timeSum = 0;
}

void Logger::beginOperation() {
  start = std::chrono::system_clock::now();
  operationsCounter++;
}

void Logger::printAvarageTime() {
  std::cout << "Avarage time: ";
  std::cout << timeSum / operationsCounter << std::endl;
}

Logger::Logger() {}
}
