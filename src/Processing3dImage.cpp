#include "Processing3dImage.h"

#include "ProcessingImage.h"
#include "ProcessingImage3d.h"
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
  img.setKernelWithOperation(operation);
}
}

void ProcessDepth::process(Image3d &image3d, ProcessingImage &img,
                           const OPERATION &operation) {
  auto &operationFun = (OperationToMethodPtr.at(operation));
  setKernelAndStructuralElement(img, operation);
  for (auto i = 0; i < image3d.getDepth(); i++) {
    img.setImageToProcess(image3d.getImageAtDepth(i).clone());
    (img.*operationFun)();
    image3d.setImageAtDepth(i, img.getImage());
  }
}

std::pair<int, int> ProcessDepth::getImageSize(const Image3d &image3d) const {
  return { image3d.getRows(), image3d.getCols() };
}

void ProcessCols::process(Image3d &image3d, ProcessingImage &img,
                          const OPERATION &operation) {
  auto &operationFun = (OperationToMethodPtr.at(operation));
  setKernelAndStructuralElement(img, operation);
  for (auto i = 0; i < image3d.getCols(); i++) {
    img.setImageToProcess(image3d.getImageAtCol(i));
    (img.*operationFun)();
    image3d.setImageAtCol(i, img.getImage());
  }
}

std::pair<int, int> ProcessCols::getImageSize(const Image3d &image3d) const {
  return { image3d.getDepth(), image3d.getRows() };
}

void ProcessDepthIn3D::process(Image3d &image3d, ProcessingImage3d &img,
                               const OPERATION &operation) {
  auto &operationFun = (OperationToMethodPtr.at(operation));
  setKernelAndStructuralElement(img, operation);
  img.set3dImageToProcess(image3d);
  (img.*operationFun)();
  image3d.set3dImage(img.getImage());
}
std::pair<int, int> ProcessDepthIn3D::getImageSize(const Image3d &) const {
  return std::make_pair(1, 1);
}
}
