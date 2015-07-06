#pragma once

#include <memory>
#include <map>
namespace Mgr {
class Image3d;
class ProcessingImage;
class ProcessingImage3d;
enum class OPERATION : uint8_t;
typedef void (ProcessingImage::*ptrToMethodType)();
extern const std::map<OPERATION, ptrToMethodType> OperationToMethodPtr;
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
class ProcessRows {
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
