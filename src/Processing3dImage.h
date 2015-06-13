#pragma once

#include <memory>
namespace Mgr {
class Image3d;
class ProcessingImage;
class ProcessingImage3d;
enum class OPERATION : uint8_t;
class ProcessCols {
public:
  void process(Image3d &image3d, ProcessingImage &img,
               const OPERATION &operation);
  std::pair<int, int> getImageSize(const Image3d &image3d) const;
};
class ProcessDepth {
public:
  void process(Image3d &image3d, ProcessingImage &img,
               const OPERATION &operation);
  std::pair<int, int> getImageSize(const Image3d &image3d) const;
};
class ProcessDepthIn3D {
public:
  void process(Image3d &image3d, ProcessingImage3d &img,
               const OPERATION &operation);
  std::pair<int, int> getImageSize(const Image3d &image3d) const;
};
}
