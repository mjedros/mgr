#ifndef PROCESSING3DIMAGE_H
#define PROCESSING3DIMAGE_H
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "Image3d.h"

namespace Mgr {
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };

class Processing3dImage {
  public:
    virtual void process(const std::shared_ptr<Mgr::Image3d> &image,
                         const std::shared_ptr<Mgr::ProcessingImage> &img,
                         const OPERATION &operation) = 0;
};
class ProcessCols : public Processing3dImage {
  public:
    void process(const std::shared_ptr<Mgr::Image3d> &image,
                 const std::shared_ptr<Mgr::ProcessingImage> &img,
                 const OPERATION &operation);
};
class ProcessDepth : public Processing3dImage {
  public:
    void process(const std::shared_ptr<Mgr::Image3d> &image,
                 const std::shared_ptr<Mgr::ProcessingImage> &img,
                 const OPERATION &operation);
};
}
#endif // PROCESSING3DIMAGE_H
