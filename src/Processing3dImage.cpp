#include "Processing3dImage.h"

#include "ProcessingImage.h"
#include "Image3d.h"
namespace Mgr {
typedef void (ProcessingImage::*ptrToMethodType)();
const std::map<OPERATION, ptrToMethodType> OperationToMethodPtr = {
  { OPERATION::DILATION, &ProcessingImage::performMorphologicalOperation },
  { OPERATION::EROSION, &ProcessingImage::performMorphologicalOperation },
  { OPERATION::CONTOUR, &ProcessingImage::contour },
  { OPERATION::SKELETONIZATION, &ProcessingImage::skeletonize }
};

namespace {
void setKernelAndStructuralElement(ProcessingImage &img,
                                   const OPERATION &operation) {
  if (operation != OPERATION::CONTOUR &&
      operation != OPERATION::SKELETONIZATION) {
    img.setKernel(operation);
    img.setStructuralElementArgument();
  }
}
}

void ProcessDepth::process(const std::shared_ptr<Image3d> &image3d,
                           ProcessingImage &img, const OPERATION &operation) {
  clear();
  auto &operationFun = (OperationToMethodPtr.at(operation));
  setKernelAndStructuralElement(img, operation);
  for (auto i = 0; i < image3d->getDepth(); i++) {
    img.setImageToProcess(image3d->getImageAtDepth(i).clone());
    (img.*operationFun)();
    image3d->setImageAtDepth(i, img.getImage());
  }
  std::cout << getAvarage() << std::endl;
}

std::pair<int, int>
ProcessDepth::getImageSize(const std::shared_ptr<Image3d> &image3d) {
  return { image3d->getRows(), image3d->getCols() };
}

void ProcessCols::process(const std::shared_ptr<Image3d> &image3d,
                          ProcessingImage &img, const OPERATION &operation) {
  auto &operationFun = (OperationToMethodPtr.at(operation));
  setKernelAndStructuralElement(img, operation);
  for (auto i = 0; i < image3d->getCols(); i++) {
    img.setImageToProcess(image3d->getImageAtCol(i));
    (img.*operationFun)();
    image3d->setImageAtCol(i, img.getImage());
  }
}

std::pair<int, int>
ProcessCols::getImageSize(const std::shared_ptr<Image3d> &image3d) {
  return { image3d->getDepth(), image3d->getRows() };
}
}
