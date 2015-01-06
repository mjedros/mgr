#pragma once
#include "IImageSource.h"
#include "FileVideo.h"
#include "Directory.h"
namespace Mgr {
/**
 * @brief Enum with types of image sources
 */
enum SourceType : u_int8_t { VideoFile, DirectorySource };
/**
 * @brief Factory pattern for images sources
 */
class SourceFactory {
  public:
    static std::unique_ptr<IImageSource>
    GetImageSource(SourceType sourceType, const std::string &fileName = "") {
        switch (sourceType) {
        case VideoFile:
            return std::unique_ptr<IImageSource>(new Mgr::FileVideo(fileName));
        case DirectorySource:
            return std::unique_ptr<IImageSource>(new Mgr::Directory(fileName));
        default:
            throw std::string("Image source type unknown");
        }
    }
};
}
