#include "Processing3dImage.h"

namespace Mgr {
typedef void (ProcessingImage::*pointerToProcessingMethodType)();
const std::map<OPERATION, pointerToProcessingMethodType>
    OperationToMethodPointerMap = {
      { OPERATION::DILATION, &ProcessingImage::dilate },
      { OPERATION::EROSION, &ProcessingImage::erode },
      { OPERATION::CONTOUR, &ProcessingImage::contour },
      { OPERATION::SKELETONIZATION, &ProcessingImage::skeletonize }
    };
void ProcessDepth::process(const std::shared_ptr<Image3d> &image,
                           const std::shared_ptr<ProcessingImage> &img,
                           const OPERATION &operation) {
  clear();
  for (auto i = 0; i < image->getDepth(); i++) {
    img->setImageToProcess(image->getImageAtDepth(i).clone());
    (img.get()->*(OperationToMethodPointerMap.at(operation)))();
    image->setImageAtDepth(i, img->getImage());
  }
  std::cout << getAvarage() << std::endl;
}

void ProcessCols::process(const std::shared_ptr<Image3d> &image,
                          const std::shared_ptr<ProcessingImage> &img,
                          const OPERATION &operation) {
  for (auto i = 0; i < image->getCols(); i++) {
    img->setImageToProcess(image->getImageAtCol(i));
    (img.get()->*(OperationToMethodPointerMap.at(operation)))();
    image->setImageAtCol(i, img->getImage());
  }
}
}
