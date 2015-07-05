#include "ContinuousProcessingMananger.h"

#include "ApplicationManager.h"
#include "Logger.h"
#include "OpenCLManager.h"
#include "ProcessingImage.h"
#include "Processing3dImage.h"
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
void ContinuousProcessingMananger::process3dImages() {
  active = true;
  std::queue<cv::Mat> *imagesSourceBuffer = &imagesQueueFirstBuffer;
  aquisitionThread.reset(new std::thread(
      &ContinuousProcessingMananger::startCameraAquisition, this));
  Image3d image3d;
  int currDepth = 0;
  while (active) {
    std::this_thread::yield();
    if (imagesSourceBuffer->empty()) {
      imagesSourceBuffer = switchBuffers();
      continue;
    }
    cv::Mat image = imagesSourceBuffer->front();
    imagesSourceBuffer->pop();
    if (!image3d.isImage3dInitialized())
      image3d = Image3d(depth, image);
    ProcessingImage processing2dImage(openCLManager);
    processing2dImage.setImageToProcess(image);
    processing2dImage.binarize();
    image3d.setImageAtDepth(currDepth, processing2dImage.getImage());

    if (++currDepth == depth) {
      currDepth = 0;
      ProcessingImage3d imageOut(openCLManager);
      imageOut.setStructuralElement("Ellipse", { 2, 2, 2 });
      logger.resetTimer();
      ProcessDepthIn3D{}.process(image3d, imageOut, OPERATION::DILATION);
      logger.printAvarageTime();
      emit(drawProcessed(image3d.getImageAtDepth(0)));
    }
  }
  aquisitionThread->join();
}
std::mutex bufferMutex;
std::queue<cv::Mat> *ContinuousProcessingMananger::switchBuffers() {
  std::lock_guard<std::mutex> lock(bufferMutex);
  if (aquisitionBuffer == &imagesQueueFirstBuffer) {
    aquisitionBuffer = &imagesQueueSeccondBuffer;
    return &imagesQueueFirstBuffer;
  }
  aquisitionBuffer = &imagesQueueFirstBuffer;
  return &imagesQueueSeccondBuffer;
}

void ContinuousProcessingMananger::startCameraAquisition() {
  aquisitionBuffer = &imagesQueueFirstBuffer;
  imagesFromCam = SourceFactory::GetImageSource(CameraSource);
  imagesFromCam->Start();
  for (Mat im = imagesFromCam->Get(); !im.empty() && active;
       im = imagesFromCam->Get()) {
    cv::cvtColor(im, im, CV_BGR2GRAY);
    {
      std::lock_guard<std::mutex> lock(bufferMutex);
      aquisitionBuffer->push(im);
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
