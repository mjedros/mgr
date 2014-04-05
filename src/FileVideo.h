#include "IImageSource.h"

#include <string>
#include <memory>
class FileVideo : public IImageSource
{
   std::string fileName;
   std::unique_ptr<cv::VideoCapture> videoCapture;

 public:
   FileVideo(const std::string &fileName);
   virtual void Start();
   virtual void Stop();
   cv::Mat Get();
};
