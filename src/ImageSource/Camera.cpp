#include "ImageSource/Camera.h"
#include "Logger.h"
#include <opencv2/opencv.hpp>
namespace Mgr {
Camera::Camera() {
  videoCapture = std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture());
}

Camera::~Camera() { Stop(); }

void Camera::Start() {
  Logger::getInstance().printLine("Start of acquisition");
  videoCapture->open(0);
}

void Camera::Stop() {
  if (!videoCapture)
    return;
  videoCapture->release();
  videoCapture.release();
}

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
