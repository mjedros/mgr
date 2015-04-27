#include "Processing3dImage.h"

#include "ProcessingImage.h"
#include "Image3d.h"
namespace Mgr {
typedef void (ProcessingImage::*pointerToProcessingMethodType)();
const std::map<OPERATION, pointerToProcessingMethodType>
    OperationToMethodPointerMap = {
      { OPERATION::DILATION, &ProcessingImage::dilate },
      { OPERATION::EROSION, &ProcessingImage::erode },
      { OPERATION::CONTOUR, &ProcessingImage::contour },
      { OPERATION::SKELETONIZATION, &ProcessingImage::skeletonize }
    };
void ProcessDepth::process(const std::shared_ptr<Image3d> &image3d,
                           ProcessingImage &img, const OPERATION &operation) {
  clear();
  for (auto i = 0; i < image3d->getDepth(); i++) {
    img.setImageToProcess(image3d->getImageAtDepth(i).clone());
    (&img->*(OperationToMethodPointerMap.at(operation)))();
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
  for (auto i = 0; i < image3d->getCols(); i++) {
    img.setImageToProcess(image3d->getImageAtCol(i));
    (&img->*(OperationToMethodPointerMap.at(operation)))();
    image3d->setImageAtCol(i, img.getImage());
  }
}

std::pair<int, int>
ProcessCols::getImageSize(const std::shared_ptr<Image3d> &image3d) {
  return { image3d->getDepth(), image3d->getRows() };
}
}
