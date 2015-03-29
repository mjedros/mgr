#ifndef CSVFILEUTILS_H
#define CSVFILEUTILS_H
#include <vector>
#include <string>
namespace Mgr {
class CsvFile {
  std::vector<std::vector<std::string>> operationsVector;

public:
  void addOperations(const std::vector<std::string> &operations);
  void saveFile(const std::string &filename);
  void loadFile(const std::string &filename);
  void clear() { operationsVector.clear(); }
  const std::vector<std::vector<std::string>> &getOperationsVector() const {
    return operationsVector;
  }
  void deleteOperation(const int &number);
};
}
#endif // CSVFILEUTILS_H
