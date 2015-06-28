#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <QObject>
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

public:
  ContinuousProcessingMananger(OpenCLManager &openCLManager, QObject *parent);
  ~ContinuousProcessingMananger() {}
  void process2dImages();

private:
  ImagesPortion getNextImagesPortionFromQueue();
  void startCameraAquisition();
  void start2dAquisition();
  void pushToQueue(ImagesPortion);
signals:
  void drawObject(cv::Mat image);
};
}
