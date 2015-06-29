#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <QObject>
#include "ImageSource/Camera.h"
#include <opencv2/opencv.hpp>

namespace Mgr {
class OpenCLManager;
using ImagesPortion = std::vector<cv::Mat *>;
class ContinuousProcessingMananger : public QObject {
  Q_OBJECT
  std::vector<cv::Mat> imagesVector;
  std::queue<ImagesPortion> portionsQueue;
  std::queue<cv::Mat> images2dQueue;

  std::unique_ptr<std::thread> aquisitionThread;
  OpenCLManager &openCLManager;
  QObject *parentObject;
  std::unique_ptr<IImageSource> imagesFromCam;
  bool active;

  std::string operationString;
  std::string MorphElementType;
  std::vector<float> StructElemParams;

public:
  ContinuousProcessingMananger(OpenCLManager &openCLManager, QObject *parent);

  void setProcessing(const std::string &operationStringNew,
                     const std::string &MorphElementTypeNew,
                     const std::vector<float> StructElemParamsNew);
  void process2dImages();
  void stopProcessing();

private:
  ImagesPortion getNextImagesPortionFromQueue();
  void startCameraAquisition();
  void start2dAquisition();
  void pushToQueue(ImagesPortion);
  void process2dImage(cv::Mat image);
signals:
  void drawObject(cv::Mat image);
  void drawProcessed(cv::Mat);
};
}
