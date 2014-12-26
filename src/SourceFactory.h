#pragma once
#include "IImageSource.h"
#include "FileVideo.h"
/**
 * @brief Enum with types of image sources
 */
enum SourceType { VideoFile };
/**
 * @brief Factory pattern for images sources
 */
class SourceFactory {
  public:
    static std::unique_ptr<IImageSource>
    GetImageSource(SourceType sourceType, const std::string &fileName = "") {
        switch (sourceType) {
        case VideoFile:
            return std::unique_ptr<IImageSource>(new FileVideo(fileName));
        default:
            throw((std::string) "Image source type unknown");
        }
    }
};
