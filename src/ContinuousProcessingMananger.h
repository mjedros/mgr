#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <opencv2/opencv.hpp>

namespace Mgr {
using ImagesPortion = std::vector<cv::Mat *>;
class ContinuousProcessingMananger {
  std::vector<cv::Mat> imagesVector;
  std::queue<ImagesPortion> portionsQueue;
  std::queue<cv::Mat> images2dQueue;

  std::unique_ptr<std::thread> aquisitionThread;

public:
  ContinuousProcessingMananger();

private:
  ImagesPortion getNextImagesPortionFromQueue();
  void startCameraAquisition();
  void start2dAquisition();
  void process2dImages();
  void pushToQueue(ImagesPortion);
};
}
