#include "FileVideo.h"
#include <opencv2/opencv.hpp>

FileVideo::FileVideo(const std::string &fileName) : fileName(fileName) {}

void FileVideo::Start()
{
   videoCapture =
       std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(fileName));
}

void FileVideo::Stop() {}

cv::Mat FileVideo::Get()
{
   cv::Mat ans;
   (*videoCapture) >> ans;
   return ans;
}
