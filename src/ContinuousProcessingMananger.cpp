#include "ContinuousProcessingMananger.h"
#include "ImageSource/Camera.h"
#include <mutex>

using cv::Mat;

namespace Mgr {
ContinuousProcessingMananger::ContinuousProcessingMananger() {}

void ContinuousProcessingMananger::startCameraAquisition() {
  imagesVector.clear();
  Camera imagesFromCam;
  const int imagesDepth = 5;
  ImagesPortion imagesPortionVect;
  int i = 0;
  for (Mat im = imagesFromCam.Get(); !im.empty(); im = imagesFromCam.Get()) {
    cv::cvtColor(im, im, CV_BGR2GRAY);
    imagesVector.push_back(im);
    imagesPortionVect.push_back(&imagesVector.back());
    ++i;
    if (i == imagesDepth) {
      pushToQueue(imagesPortionVect);
      imagesPortionVect.clear();
      i = 0;
    }
  }
}
std::mutex images2dMutex;
void ContinuousProcessingMananger::start2dAquisition() {
  Camera imagesFromCam;
  for (Mat im = imagesFromCam.Get(); !im.empty(); im = imagesFromCam.Get()) {
    cv::cvtColor(im, im, CV_BGR2GRAY);
    {
      std::lock_guard<std::mutex> lock(images2dMutex);
      images2dQueue.push(im);
    }
  }
}

void ContinuousProcessingMananger::process2dImages() {

}

std::mutex imagesPortionMutex;
ImagesPortion ContinuousProcessingMananger::getNextImagesPortionFromQueue() {
  std::lock_guard<std::mutex> lock(imagesPortionMutex);
  if (portionsQueue.empty())
    return ImagesPortion();
  ImagesPortion imagesPortion = portionsQueue.front();
  portionsQueue.pop();
  return imagesPortion;
}
void
ContinuousProcessingMananger::pushToQueue(ImagesPortion imagesPortionVect) {
  std::lock_guard<std::mutex> lock(imagesPortionMutex);
  portionsQueue.push(imagesPortionVect);
}
}
