#include "IImageSource.h"

#include <string>
#include <memory>
#include <set>
namespace Mgr {
class Directory : public IImageSource {
    /**
     * @brief Video filename
     */
    std::set<std::string>::const_iterator it;
    std::string directory;
    /**
     * @brief Video capture ptr
     */
    std::set<std::string> files;

  public:
    /**
     * @brief Constructor
     * @param Filename
     */
    Directory(const std::string &directory);
    virtual void Start();
    virtual void Stop();
    cv::Mat Get();
};
}
