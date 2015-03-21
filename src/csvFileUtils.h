#ifndef CSVFILEUTILS_H
#define CSVFILEUTILS_H
#include <vector>
#include <string>
namespace Mgr {
const std::vector<std::vector<std::string>>
getOperationsVector(const std::string &filename);
}
#endif // CSVFILEUTILS_H
