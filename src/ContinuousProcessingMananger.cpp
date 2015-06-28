#include "ContinuousProcessingMananger.h"

#include "ApplicationManager.h"
#include "Logger.h"
#include "OpenCLManager.h"
#include "GUI/cvImageWindow.h"
#include "ImageSource/Camera.h"
#include <mutex>
#include <chrono>

using cv::Mat;

namespace Mgr {

static Logger &logger = Logger::getInstance();

ContinuousProcessingMananger::ContinuousProcessingMananger(
    OpenCLManager &openCLManager, QObject *parent)
  : openCLManager(openCLManager), parentObject(parent) {
  qRegisterMetaType<cv::Mat>("a");
  connect(this, SIGNAL(drawObject(cv::Mat)), parentObject,
          SLOT(drawObject(cv::Mat)));
}

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
  logger.printLine("Started aquisition thread");
  Camera imagesFromCam;
  imagesFromCam.Start();
  for (Mat im = imagesFromCam.Get(); !im.empty(); im = imagesFromCam.Get()) {
    cv::cvtColor(im, im, CV_BGR2GRAY);
    {
      std::lock_guard<std::mutex> lock(images2dMutex);
      images2dQueue.push(im);
      emit(drawObject(im));
    }
  }
  logger.printLine("End of aquisition thread");
}

void ContinuousProcessingMananger::process2dImages() {
  logger.printLine("Starting processing 2d image from camera");
  aquisitionThread = std::unique_ptr<std::thread>(
      new std::thread(&ContinuousProcessingMananger::start2dAquisition, this));
  std::this_thread::sleep_for(std::chrono::microseconds(1));
  cv::Mat im;
  for (;;) {
    {
      std::lock_guard<std::mutex> lock(images2dMutex);
      if (images2dQueue.empty())
        break;
      im = images2dQueue.front();
      images2dQueue.pop();
    }
    std::this_thread::yield();
  }
  logger.printLine("End of processing");
  aquisitionThread->join();
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
