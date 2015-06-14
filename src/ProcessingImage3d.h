#pragma once

#include "ProcessingImage.h"
#include <memory>

namespace Mgr {
class OpenCLManager;
class ProcessingImage3d : public ProcessingImage {
public:
  ProcessingImage3d(const std::shared_ptr<OpenCLManager> &openCLManagerPtr,
                    bool processRoi = false);

  void set3dImageToProcess(const Image3d &image3d);
  void setKernel(const std::string &Operation) override;
  void setStructuralElementArgument() override;
  void performMorphologicalOperation() override;
};
} // Mgr namespace
