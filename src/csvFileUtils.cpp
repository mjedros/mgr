#include "csvFileUtils.h"
#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>

using namespace std;
namespace Mgr {
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
}
