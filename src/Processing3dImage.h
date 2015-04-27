#pragma once

#include <memory>

namespace Mgr {
class Image3d;
class ProcessingImage;
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };

class ProcessCols {
public:
  void process(const std::shared_ptr<Image3d> &image3d, ProcessingImage &img,
               const OPERATION &operation);
  std::pair<int, int> getImageSize(const std::shared_ptr<Image3d> &image3d);
};
class ProcessDepth {
public:
  void process(const std::shared_ptr<Image3d> &image3d, ProcessingImage &img,
               const OPERATION &operation);
  std::pair<int, int> getImageSize(const std::shared_ptr<Image3d> &image3d);
};
}
