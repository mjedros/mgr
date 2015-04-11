#ifndef PROCESSING3DIMAGE_H
#define PROCESSING3DIMAGE_H
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "Image3d.h"

namespace Mgr {
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };

class ProcessCols {
public:
  void process(const std::shared_ptr<Mgr::Image3d> &image3d,
               const std::shared_ptr<Mgr::ProcessingImage> &img,
               const OPERATION &operation);
};
class ProcessDepth {
public:
  void process(const std::shared_ptr<Mgr::Image3d> &image3d,
               const std::shared_ptr<Mgr::ProcessingImage> &img,
               const OPERATION &operation);
};
}
#endif // PROCESSING3DIMAGE_H
