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
static const unsigned int depth = 10;
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
std::mutex aquisitionMutex;
bool ContinuousProcessingMananger::switchBuffers() {
  if (imagesCounter % depth != 0 || imagesCounter == 0)
    return false;
  std::lock_guard<std::mutex> lock(aquisitionMutex);
  imagesCounter = 0;
  if (aquisitionBuffer == &imagesVectorFirstBuffer)
    aquisitionBuffer = &imagesVectorSeccondBuffer;
  else
    aquisitionBuffer = &imagesVectorFirstBuffer;
  return true;
}

void ContinuousProcessingMananger::startCameraAquisition() {
  imagesVectorFirstBuffer.clear();
  aquisitionBuffer = &imagesVectorFirstBuffer;
  imagesVectorSeccondBuffer.clear();
  imagesFromCam = SourceFactory::GetImageSource(CameraSource);
  imagesFromCam->Start();
  imagesCounter = 0;
  for (Mat im = imagesFromCam->Get(); !im.empty() && active;
       im = imagesFromCam->Get()) {
    cv::cvtColor(im, im, CV_BGR2GRAY);
    {
      std::lock_guard<std::mutex> lock(aquisitionMutex);
      aquisitionBuffer->push_back(im);
      ++imagesCounter;
    }
    emit(drawObject(im));
  }
  imagesFromCam->Stop();
  logger.printLine("End of acquisition");
}

void ContinuousProcessingMananger::process2dImage(cv::Mat image) {
  ProcessingImage img(openCLManager);
  img.setImageToProcess(image);
  img.setStructuralElement(MorphElementType, StructElemParams);
  img.dilate();
  emit(drawProcessed(img.getImage()));
}

void ContinuousProcessingMananger::process2dImages() {
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

void ContinuousProcessingMananger::process3dImages() {
  active = true;
  aquisitionThread.reset(new std::thread(
      &ContinuousProcessingMananger::startCameraAquisition, this));
  while (active) {
    std::this_thread::yield();
    if (switchBuffers()) {
      std::cout << "Buffers switched" << std::endl;
    }
  }
  aquisitionThread->join();
}

void ContinuousProcessingMananger::stopProcessing() {
  if (!active)
    return;
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
