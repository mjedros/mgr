#include "Logger.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;
namespace Mgr {
void Logger::printError(cl::Error &err) {
  file << err.what() << endl;
  file << err.err() << endl;
}

void Logger::printText(string text) { file << text; }

void Logger::printLine(string line) {
  printText(line);
  file << endl;
}

void Logger::printProcessingROI(bool roi) {
  if (roi)
    printText(", processing roi");
  file << endl;
}

void Logger::endOperation() {
  auto end = chrono::system_clock::now();
  timeSum += chrono::duration_cast<chrono::microseconds>(end - start).count();
}

void Logger::resetTimer() {
  operationsCounter = 0;
  timeSum = 0;
}

void Logger::beginOperation() {
  start = chrono::system_clock::now();
  operationsCounter++;
}

void Logger::printAvarageTime() {
  double inMiliseconds = timeSum / 1000.0;
  file << "Full time:" << timeSum << " us, " << inMiliseconds << " ms" << endl;
  file << "Avarage time: ";
  file << timeSum / operationsCounter << " us, "
       << inMiliseconds / operationsCounter << " ms" << endl;
  file << endl;
}

void Logger::printFancyLine(string line) {
  file << "******************************" << endl;
  printLine(line);
}

Logger::Logger() {
  file.open("logMgr.tx", ios::out | fstream::app);
  file << "***************** New log *****************" << endl;
  const auto now = chrono::system_clock::now();
  const auto in_time_t = chrono::system_clock::to_time_t(now);
  file << ctime(&in_time_t);
}

Logger::~Logger() { file.close(); }
}
