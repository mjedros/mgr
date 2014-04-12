#pragma once
#include "IImageSource.h"
#include "FileVideo.h"

enum SourceType
{
   VideoFile
};

class SourceFactory
{
 public:
   static std::unique_ptr<IImageSource> GetImageSource(SourceType sourceType,
                                                       const std::string &fileName = "")
   {
      switch (sourceType)
      {
      case VideoFile:
         return std::unique_ptr<IImageSource>(new FileVideo(fileName));
      default:
         throw((std::string) "Image source type unknown");
      }
   }
};
