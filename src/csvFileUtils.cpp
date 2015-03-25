#include "csvFileUtils.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>

using namespace std;
namespace Mgr {
namespace Utils {
const vector<string> split(const string &text, char sep) {
  vector<string> tokens;
  size_t start = 0, end = 0;
  while ((end = text.find(sep, start)) != string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));
  return tokens;
}

const vector<vector<string>> getOperationsVector(const string &filename) {
  ifstream csvFile(filename);
  string line;

  vector<vector<string>> operationsVector;
  while (getline(csvFile, line))
    operationsVector.push_back(split(line, ','));

  return operationsVector;
}

void saveCsvFile(const vector<vector<string>> &operationsVector,
                 const string &filename) {
  ofstream csvFile(filename);
  for (auto &operation : operationsVector) {
    for (auto token = operation.begin(); token != operation.end(); token++) {
      csvFile << *token;
      if (token != operation.end() - 1)
        csvFile << ",";
    }
    csvFile << endl;
  }
}
} // namespace Utils
void CsvFile::addOperations(const vector<string> &operations) {
  operationsVector.push_back(operations);
}

void CsvFile::saveFile(const string &filename) {
  Utils::saveCsvFile(operationsVector, filename);
}

void CsvFile::loadFile(const string &filename) {
  operationsVector = Utils::getOperationsVector(filename);
}
}
