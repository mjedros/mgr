#include "IImageSource.h"

#include <string>
#include <memory>
#include <set>
namespace Mgr {
/**
 * @brief Class representing directory with images as image source
 *
 */
class Directory : public IImageSource {
  /**
   * @brief Const iterator
   */
  std::set<std::string>::const_iterator it;
  /**
   * @brief Directory
   */
  std::string directory;
  /**
   * @brief set of files
   */
  std::set<std::string> files;

public:
  /**
   * @brief Constructor
   * @param directory
   */
  Directory(const std::string &directory);
  virtual void Start();
  virtual void Stop();
  cv::Mat Get();
};
}
