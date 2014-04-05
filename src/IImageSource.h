#pragma once

namespace cv
{
class Mat;
class VideoCapture;
}

class IImageSource
{
 public:
   virtual void Start() = 0;
   virtual void Stop() = 0;
   virtual cv::Mat Get() = 0;
};
