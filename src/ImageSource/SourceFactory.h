#pragma once

#include "ImageSource/Camera.h"
#include "ImageSource/IImageSource.h"
#include "ImageSource/FileVideo.h"
#include "ImageSource/Directory.h"
namespace Mgr {
/**
 * @brief Enum with types of image sources
 */
enum SourceType : u_int8_t { VideoFile, DirectorySource, CameraSource };
/**
 * @brief Factory pattern for images sources
 */
class SourceFactory {
public:
  static std::unique_ptr<IImageSource>
  GetImageSource(SourceType sourceType, const std::string &name = "") {
    switch (sourceType) {
    case VideoFile:
      return std::unique_ptr<IImageSource>(new FileVideo(name));
    case DirectorySource:
      return std::unique_ptr<IImageSource>(new Directory(name));
    case CameraSource:
      return std::unique_ptr<IImageSource>(new Camera);
    default:
      throw std::string("Image source type unknown");
    }
  }
};
}
