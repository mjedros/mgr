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

void Logger::endOperation(const unsigned long int &clDuration) {
  auto end = chrono::system_clock::now();
  timeSum += chrono::duration_cast<chrono::microseconds>(end - start).count();
  clSum += clDuration;
}

void Logger::resetTimer() {
  operationsCounter = 0;
  timeSum = 0;
  allSum = chrono::system_clock::now();
}

void Logger::beginOperation() {
  start = chrono::system_clock::now();
  clSum = 0;
  operationsCounter++;
}

void Logger::printAvarageTime() {
  auto end = chrono::system_clock::now();
  unsigned int allSumTime =
      chrono::duration_cast<chrono::microseconds>(end - allSum).count();
  double inMiliseconds = timeSum / 1000.0;
  file << "All time: " << allSumTime << " us, " << allSumTime / 1000.0 << " ms"
       << endl;
  file << "Full time: " << timeSum << " us, " << inMiliseconds << " ms" << endl;
  file << "Avarage time: ";
  file << timeSum / operationsCounter << " us, "
       << inMiliseconds / operationsCounter << " ms" << endl;
  file << "Avarage OpenCL profiling: " << clSum << " ns, " << clSum / 1000000.0
       << " ms" << endl;
  file << endl;
}

void Logger::printFancyLine(string line) {
  file << "******************************" << endl;
  printLine(line);
}

void Logger::printProcessing(const std::string &operationString,
                             const std::string &MorphElementType,
                             const std::vector<float> StructElemParams,
                             const std::string &operationWay, bool processROI) {
  printFancyLine("Performing " + operationString);
  printLine(operationWay + ", " + MorphElementType);
  printLine("Struct elem params: " + std::to_string(StructElemParams[0]) +
            ", " + std::to_string(StructElemParams[1]) + ", " +
            std::to_string(StructElemParams[2]));
  printProcessingROI(processROI);
}

Logger::Logger() {
  file.open("logMgr.txt", ios::out | fstream::app);
  file << "***************** New log *****************" << endl;
  const auto now = chrono::system_clock::now();
  const auto in_time_t = chrono::system_clock::to_time_t(now);
  file << ctime(&in_time_t);
}

Logger::~Logger() { file.close(); }
}
