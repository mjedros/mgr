#pragma once

#include <vector>
#include <string>
namespace Mgr {
using OperationsVector = std::vector<std::vector<std::string>>;
class CsvFile {
  OperationsVector operationsVector;

public:
  void addOperations(const std::vector<std::string> &operations);
  void saveFile(const std::string &filename);
  void loadFile(const std::string &filename);
  void clear() { operationsVector.clear(); }
  const OperationsVector &getOperationsVector() const {
    return operationsVector;
  }
  void deleteOperation(const int &number);
};
}
