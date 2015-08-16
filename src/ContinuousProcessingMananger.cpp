#include "ContinuousProcessingMananger.h"

#include "ApplicationManager.h"
#include "Logger.h"
#include "OpenCLManager.h"
#include "ProcessingImage.h"
#include "Processing3dImage.h"
#include "GUI/cvImageWindow.h"
#include "GUI/vtkview.h"
#include "QVTKWidget.h"
#include "ImageSource/Camera.h"
#include "ImageSource/SourceFactory.h"
#include <mutex>
#include <chrono>

using cv::Mat;
namespace Mgr {
static const unsigned int depth = 15;
static Logger &logger = Logger::getInstance();

ContinuousProcessingMananger::ContinuousProcessingMananger(
    OpenCLManager &openCLManager, QObject *parent)
  : openCLManager(openCLManager), parentObject(parent) {
  qRegisterMetaType<cv::Mat>("cv::Mat");
  qRegisterMetaType<std::shared_ptr<Mgr::Image3d>>(
      "std::shared_ptr<Mgr::Image3d>");
  connect(this, SIGNAL(drawObject(cv::Mat)), parentObject,
          SLOT(drawObject(cv::Mat)));
  connect(this, SIGNAL(drawProcessed(cv::Mat)), parentObject,
          SLOT(drawProcessed(cv::Mat)));
  connect(this, SIGNAL(showVtkImage(const std::shared_ptr<Mgr::Image3d> &)),
          parentObject,
          SLOT(drawVtkImage(const std::shared_ptr<Mgr::Image3d> &)));
}

void ContinuousProcessingMananger::setProcessing(
    const std::string &operationStringNew,
    const std::string &MorphElementTypeNew,
    const std::vector<float> StructElemParamsNew, const unsigned int &minumum,
    const unsigned int &maximumum)

{
  operationString = operationStringNew;
  MorphElementType = MorphElementTypeNew;
  StructElemParams = StructElemParamsNew;
  min = minumum;
  max = maximumum;
}

std::mutex bufferMutex;
void ContinuousProcessingMananger::process3dImage() {
  logger.printProcessing(operationString, MorphElementType, StructElemParams,
                         "3d processing");
  ProcessingImage3d imageOut(openCLManager);
  imageOut.setStructuralElement(MorphElementType, StructElemParams);
  logger.resetTimer();
  ProcessDepthIn3D{}.process(*image3d, imageOut,
                             ProcessingImage3d::OperationMap[operationString]);
  logger.endOperation();
  logger.printAvarageTime();
  std::this_thread::yield();
}

void ContinuousProcessingMananger::processing3dLoop() {
  MatQueue *imagesSourceBuffer = &imagesQueueFirstBuffer;
  int currDepth = 0;
  while (active) {
    std::this_thread::yield();
    {
      std::lock_guard<std::mutex> lock(bufferMutex);
      if (imagesSourceBuffer->empty()) {
        imagesSourceBuffer = switchBuffers();
        continue;
      }
    }
    cv::Mat image = imagesSourceBuffer->front();
    imagesSourceBuffer->pop();
    if (!image3d->isImage3dInitialized())
      image3d = std::shared_ptr<Image3d>(new Image3d(depth, image));
    ProcessingImage processing2dImage(openCLManager);
    processing2dImage.setImageToProcess(image.clone());
    processing2dImage.binarize(min, max);
    image3d->setImageAtDepth(currDepth, processing2dImage.getImage());

    if (++currDepth == depth) {
      currDepth = 0;
      process3dImage();
      emit(showVtkImage(image3d));
    }
  }
}

void ContinuousProcessingMananger::process3dImages() {
  active = true;
  image3d = std::shared_ptr<Image3d>(new Image3d);

  aquisitionThread.reset(new std::thread(
      &ContinuousProcessingMananger::startCameraAquisition, this));
  processing3dLoop();
  aquisitionThread->join();
  logger.printLine("End of processing");
}

MatQueue *ContinuousProcessingMananger::switchBuffers() {
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
  Mat emptyImage = imagesFromCam->Get();
  for (Mat im = imagesFromCam->Get(); !im.empty() && active;
       im = imagesFromCam->Get()) {
    // im = emptyImage - im;
    cv::cvtColor(im, im, CV_BGR2GRAY);
    {
      std::lock_guard<std::mutex> lock(bufferMutex);
      aquisitionBuffer->push(im);
    }
    std::this_thread::yield();
    emit(drawObject(im));
  }
  imagesFromCam->Stop();
  active = false;
  logger.printLine("End of acquisition");
}

void ContinuousProcessingMananger::process2dImage(cv::Mat image) {
  ProcessingImage img(openCLManager);
  img.setImageToProcess(image);
  img.setStructuralElement(MorphElementType, StructElemParams);
  auto operation = ProcessingImage3d::OperationMap[operationString];
  img.setKernelWithOperation(operation);
  (img.*OperationToMethodPtr.at(operation))();
  emit(drawProcessed(img.getImage()));
}

void ContinuousProcessingMananger::process2dImages() {
  logger.printLine("Starting processing 2d image from camera");
  active = true;
  aquisitionThread.reset(new std::thread(
      &ContinuousProcessingMananger::startCameraAquisition, this));

  MatQueue *imagesSourceBuffer = &imagesQueueFirstBuffer;

  cv::BackgroundSubtractorMOG2 bg(6, 10, false);
  bg.set("nmixtures", 3);

  while (active) {

    std::this_thread::yield();
    {
      std::lock_guard<std::mutex> lock(bufferMutex);
      if (imagesSourceBuffer->empty()) {
        imagesSourceBuffer = switchBuffers();
        std::this_thread::yield();
        continue;
      }
    }
    cv::Mat image = imagesSourceBuffer->front();
    imagesSourceBuffer->pop();
    cv::Mat fore;
    bg.operator()(image, fore);
    ProcessingImage img(openCLManager);
    img.setImageToProcess(fore.clone());
    //    img.binarize(min, max);
    process2dImage(img.getImage());
  }

  aquisitionThread->join();
  logger.printLine("End of processing");
}

void ContinuousProcessingMananger::stopProcessing() { active = false; }
}
