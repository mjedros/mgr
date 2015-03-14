#include "IImageSource.h"

#include <string>
#include <memory>
#include <set>
namespace Mgr {
/**
 * @brief Class representing directory with images as image source
 *
 */

enum fileType : u_int8_t { DICOM, OTHER };

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
   * @brief Video capture ptr
   */
  std::set<std::string> files;
  fileType type;

  cv::Mat getDicomImage(const std::string &filename);

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
