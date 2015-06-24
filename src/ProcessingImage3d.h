#pragma once

#include "ProcessingImage.h"
#include "Image3d.h"
#include <memory>

namespace Mgr {
class OpenCLManager;
class ProcessingImage3d : public ProcessingImage {
  cv::Mat ellipsoidImage;
  Image3d image3d;
  Image3d roiImage3d;
  std::unique_ptr<cl::Image3D> ellipseIn3d;

public:
  ProcessingImage3d(OpenCLManager &openCLManagerRef, bool processRoi = false);

  void set3dImageToProcess(const Image3d &image3d);
  void setKernel(const std::string &Operation) override;
  void setStructuralElementArgument() override;
  void performMorphologicalOperation() override;

  void getROIOOutOfMat() override;
  void updateFullImage() override;

  void performOperation(cl::Image3D &image_out3d);
};
} // Mgr namespace
