#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <QObject>
#include "ImageSource/Camera.h"
#include "GUI/vtkview.h"
#include <opencv2/opencv.hpp>

class VTKView;

namespace Mgr {
class OpenCLManager;
using MatQueue = std::queue<cv::Mat>;
class ContinuousProcessingMananger : public QObject {
  Q_OBJECT
  MatQueue imagesQueueFirstBuffer;
  MatQueue imagesQueueSeccondBuffer;
  MatQueue *aquisitionBuffer;

  std::unique_ptr<VTKView> vtkView;
  std::shared_ptr<Image3d> image3d;

  std::unique_ptr<std::thread> aquisitionThread;
  OpenCLManager &openCLManager;
  QObject *parentObject;
  std::unique_ptr<IImageSource> imagesFromCam;
  bool active;

  std::string operationString;
  std::string MorphElementType;
  std::vector<float> StructElemParams;
  unsigned int imagesCounter = 0;
  unsigned int min;
  unsigned int max;

public:
  ContinuousProcessingMananger(OpenCLManager &openCLManager, QObject *parent);

  void setProcessing(const std::string &operationStringNew,
                     const std::string &MorphElementTypeNew,
                     const std::vector<float> StructElemParamsNew,
                     const unsigned int &minumum,
                     const unsigned int &maximumum);
  void process2dImages();
  void process3dImages();
  void stopProcessing();

private:
  void startCameraAquisition();
  void process2dImage(cv::Mat image);
  void process3dImage();
  void processing3dLoop();
  MatQueue *switchBuffers();
signals:
  void drawObject(cv::Mat image);
  void drawProcessed(cv::Mat);
  void showVtkImage(const std::shared_ptr<Mgr::Image3d> &);
};
}
