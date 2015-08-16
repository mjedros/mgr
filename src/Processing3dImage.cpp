#include "Processing3dImage.h"

#include "ProcessingImage.h"
#include "ProcessingImage3d.h"
#include "Image3d.h"
#include "Logger.h"

namespace Mgr {
static Logger &logger = Logger::getInstance();

const std::map<OPERATION, ptrToMethodType> OperationToMethodPtr = {
  { OPERATION::DILATION, &ProcessingImage::performMorphologicalOperation },
  { OPERATION::EROSION, &ProcessingImage::performMorphologicalOperation },
  { OPERATION::CONTOUR, &ProcessingImage::contour },
  { OPERATION::OPENING, &ProcessingImage::open },
  { OPERATION::CLOSING, &ProcessingImage::close },
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
  std::string size = "Size = " + std::to_string(image3d.getCols()) + "x" +
                     std::to_string(image3d.getRows());
  logger.printLine(size);
  auto operationFun = prepareKernelAndGetMethod(img, operation);
  for (auto i = 0; i < image3d.getDepth(); i++) {
    img.setImageToProcess(image3d.getImageAtDepth(i));
    (img.*operationFun)();
    image3d.setImageAtDepth(i, img.getImage());
  }
}

void ProcessDepth::processSequence(Image3d &image3d, ProcessingImage &img,
                                   const OperationsVector &operationsVect) {
  for (auto i = 0; i < image3d.getDepth(); i++) {
    img.setImageToProcess(image3d.getImageAtDepth(i).clone());
    img.processSequence(operationsVect);
    image3d.setImageAtDepth(i, img.getImage());
  }
}

std::pair<int, int> ProcessDepth::getImageSize(const Image3d &image3d) const {
  return { image3d.getCols(), image3d.getRows() };
}

void ProcessCols::process(Image3d &image3d, ProcessingImage &img,
                          const OPERATION &operation) {
  std::string size = "Size = " + std::to_string(image3d.getDepth()) + "x" +
                     std::to_string(image3d.getRows());
  logger.printLine(size);
  auto operationFun = prepareKernelAndGetMethod(img, operation);
  for (auto i = 0; i < image3d.getCols(); i++) {
    img.setImageToProcess(image3d.getImageAtCol(i).clone());
    (img.*operationFun)();
    image3d.setImageAtCol(i, img.getImage());
  }
}

void ProcessCols::processSequence(Image3d &image3d, ProcessingImage &img,
                                  const OperationsVector &operationsVect) {
  for (auto i = 0; i < image3d.getCols(); i++) {
    img.setImageToProcess(image3d.getImageAtCol(i).clone());
    img.processSequence(operationsVect);
    image3d.setImageAtCol(i, img.getImage());
  }
}

std::pair<int, int> ProcessCols::getImageSize(const Image3d &image3d) const {
  return { image3d.getDepth(), image3d.getRows() };
}

void ProcessRows::process(Image3d &image3d, ProcessingImage &img,
                          const OPERATION &operation) {
  std::string size = "Size = " + std::to_string(image3d.getCols()) + "x" +
                     std::to_string(image3d.getDepth());
  logger.printLine(size);
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

void ProcessRows::processSequence(Image3d &image3d, ProcessingImage &img,
                                  const OperationsVector &operationsVect) {
  for (auto i = 0; i < image3d.getRows(); i++) {
    img.setImageToProcess(image3d.getImageAtRow(i));
    img.processSequence(operationsVect);
    image3d.setImageAtRow(i, img.getImage());
  }
}

void ProcessDepthIn3D::process(Image3d &image3d, ProcessingImage3d &img,
                               const OPERATION &operation) {
  std::string size = "Size = " + std::to_string(image3d.getCols()) + "x" +
                     std::to_string(image3d.getRows()) + "x" +
                     std::to_string(image3d.getDepth());
  logger.printLine(size);
  auto operationFun = prepareKernelAndGetMethod(img, operation);
  img.set3dImageToProcess(image3d);
  (img.*operationFun)();
  image3d.set3dImage(img.getImage());
}

void ProcessDepthIn3D::processSequence(Image3d &image3d, ProcessingImage3d &img,
                                       const OperationsVector &operationsVect) {
  img.set3dImageToProcess(image3d);
  img.processSequence(operationsVect);
  image3d.set3dImage(img.getImage());
}
std::pair<int, int>
ProcessDepthIn3D::getImageSize(const Image3d &image3d) const {
  return { image3d.getCols(), image3d.getRows() };
}
}
