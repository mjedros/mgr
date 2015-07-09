#include "Processing3dImage.h"

#include "ProcessingImage.h"
#include "ProcessingImage3d.h"
#include "Image3d.h"
namespace Mgr {
const std::map<OPERATION, ptrToMethodType> OperationToMethodPtr = {
  { OPERATION::DILATION, &ProcessingImage::performMorphologicalOperation },
  { OPERATION::EROSION, &ProcessingImage::performMorphologicalOperation },
  { OPERATION::CONTOUR, &ProcessingImage::contour },
  { OPERATION::SKELETONIZATION, &ProcessingImage::skeletonize },
  { OPERATION::SKELETONIZATION2, &ProcessingImage::skeletonize2 }
};

namespace {
ptrToMethodType prepareKernelAndGetMethod(ProcessingImage &img,
                                          const OPERATION &operation) {
  img.setKernelWithOperation(operation);
  return (OperationToMethodPtr.at(operation));
}
}

void ProcessDepth::process(Image3d &image3d, ProcessingImage &img,
                           const OPERATION &operation) {
  auto operationFun = prepareKernelAndGetMethod(img, operation);
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
  auto operationFun = prepareKernelAndGetMethod(img, operation);
  for (auto i = 0; i < image3d.getCols(); i++) {
    img.setImageToProcess(image3d.getImageAtCol(i));
    (img.*operationFun)();
    image3d.setImageAtCol(i, img.getImage());
  }
}

std::pair<int, int> ProcessCols::getImageSize(const Image3d &image3d) const {
  return { image3d.getDepth(), image3d.getRows() };
}

void ProcessRows::process(Image3d &image3d, ProcessingImage &img,
                          const OPERATION &operation) {
  auto operationFun = prepareKernelAndGetMethod(img, operation);
  for (auto i = 0; i < image3d.getRows(); i++) {
    img.setImageToProcess(image3d.getImageAtRow(i));
    (img.*operationFun)();
    image3d.setImageAtRow(i, img.getImage());
  }
}

std::pair<int, int> ProcessRows::getImageSize(const Image3d &image3d) const {
  return { image3d.getCols(), image3d.getDepth() };
}

void ProcessDepthIn3D::process(Image3d &image3d, ProcessingImage3d &img,
                               const OPERATION &operation) {
  auto operationFun = prepareKernelAndGetMethod(img, operation);
  img.set3dImageToProcess(image3d);
  (img.*operationFun)();
  image3d.set3dImage(img.getImage());
}
std::pair<int, int>
ProcessDepthIn3D::getImageSize(const Image3d &image3d) const {
  return std::make_pair(image3d.getRows(), image3d.getCols());
}
}
