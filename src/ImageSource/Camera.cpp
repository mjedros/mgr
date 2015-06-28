#include "ImageSource/Camera.h"

#include <opencv2/opencv.hpp>
namespace Mgr {
Camera::Camera() {
  videoCapture = std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture());
}

void Camera::Start() { videoCapture->open(0); }

void Camera::Stop() { videoCapture->release(); }

cv::Mat Camera::Get() {
  if (videoCapture->isOpened()) {
    cv::Mat outImage;
    *videoCapture >> outImage;
    if (outImage.empty())
      Stop();
    return outImage;
  }
  return cv::Mat();
}
}
