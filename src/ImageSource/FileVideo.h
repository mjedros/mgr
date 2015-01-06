#include "IImageSource.h"

#include <string>
#include <memory>
#include <set>
namespace Mgr {
/**
 * @brief Class representing video file as image source
 *
 */
class FileVideo : public IImageSource {
    /**
     * @brief Video filename
     */
    std::string fileName;
    /**
     * @brief Video capture ptr
     */
    std::unique_ptr<cv::VideoCapture> videoCapture;

  public:
    /**
     * @brief Constructor
     * @param Filename
     */
    FileVideo(const std::string &fileName);
    virtual void Start();
    virtual void Stop();
    cv::Mat Get();
};
}
