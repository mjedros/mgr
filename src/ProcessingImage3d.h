#pragma once

#include "ProcessingImage.h"
#include <memory>

namespace Mgr {
class OpenCLManager;
class ProcessingImage3d : public ProcessingImage {
public:
  ProcessingImage3d(const std::shared_ptr<OpenCLManager> &openCLManagerPtr,
                    bool processRoi = false);
  void setKernel(const std::string &Operation) override;
  void set3dImageToProcess(const Image3d &image3d);
};
} // Mgr namespace
