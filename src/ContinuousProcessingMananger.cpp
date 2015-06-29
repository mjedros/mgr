#include "ContinuousProcessingMananger.h"

#include "ApplicationManager.h"
#include "Logger.h"
#include "OpenCLManager.h"
#include "ProcessingImage.h"
#include "GUI/cvImageWindow.h"
#include "ImageSource/Camera.h"
#include "ImageSource/SourceFactory.h"
#include <mutex>
#include <chrono>

using cv::Mat;

namespace Mgr {

static Logger &logger = Logger::getInstance();

ContinuousProcessingMananger::ContinuousProcessingMananger(
    OpenCLManager &openCLManager, QObject *parent)
  : openCLManager(openCLManager), parentObject(parent) {
  qRegisterMetaType<cv::Mat>("cv::Mat");
  connect(this, SIGNAL(drawObject(cv::Mat)), parentObject,
          SLOT(drawObject(cv::Mat)));
  connect(this, SIGNAL(drawProcessed(cv::Mat)), parentObject,
          SLOT(drawProcessed(cv::Mat)));
}

void ContinuousProcessingMananger::setProcessing(
    const std::string &operationStringNew,
    const std::string &MorphElementTypeNew,
    const std::vector<float> StructElemParamsNew) {
  operationString = operationStringNew;
  MorphElementType = MorphElementTypeNew;
  StructElemParams = StructElemParamsNew;
}

void ContinuousProcessingMananger::startCameraAquisition() {
  imagesVector.clear();
  const int imagesDepth = 5;
  ImagesPortion imagesPortionVect;
  int i = 0;
  imagesFromCam = SourceFactory::GetImageSource(CameraSource);
  imagesFromCam->Start();
  for (Mat im = imagesFromCam->Get(); !im.empty(); im = imagesFromCam->Get()) {
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
  std::unique_ptr<IImageSource> imagesFromCam =
      SourceFactory::GetImageSource(CameraSource);

  imagesFromCam->Start();
  for (Mat im = imagesFromCam->Get(); !im.empty() && active;
       im = imagesFromCam->Get()) {
    cv::cvtColor(im, im, CV_BGR2GRAY);
    {
      std::lock_guard<std::mutex> lock(images2dMutex);
      images2dQueue.push(im);
      emit(drawObject(im));
      std::this_thread::yield();
    }
  }
  logger.printLine("End of aquisition thread");
}

void ContinuousProcessingMananger::process2dImage(cv::Mat image) {
  ProcessingImage img(openCLManager);
  img.setImageToProcess(image);
  img.setStructuralElement(MorphElementType, StructElemParams);
  img.dilate();
  emit(drawProcessed(img.getImage()));
}

void ContinuousProcessingMananger::process2dImages() {
  active = true;
  logger.printLine("Starting processing 2d image from camera");
  imagesFromCam = SourceFactory::GetImageSource(CameraSource);

  imagesFromCam->Start();
  for (Mat im = imagesFromCam->Get(); !im.empty() && active;
       im = imagesFromCam->Get()) {
    cv::cvtColor(im, im, CV_BGR2GRAY);
    emit(drawObject(im));
    ProcessingImage img(openCLManager);
    img.setImageToProcess(im.clone());
    img.binarize();
    process2dImage(img.getImage());
  }
  logger.printLine("End of processing");
}

void ContinuousProcessingMananger::stopProcessing() {
  if (!active)
    return;
  imagesFromCam->Stop();
  active = false;
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
