#include "ImageSource/FileVideo.h"

#include <opencv2/opencv.hpp>
namespace Mgr {
FileVideo::FileVideo(const std::string &fileName) : fileName(fileName) {}

void FileVideo::Start() { videoCapture.reset(new cv::VideoCapture(fileName)); }

void FileVideo::Stop() {}

cv::Mat FileVideo::Get() {

  cv::Mat ans;
  (*videoCapture) >> ans;
  return ans;
}
}
