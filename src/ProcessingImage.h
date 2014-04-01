#ifndef IMAGE_H
#define IMAGE_H
#include <opencv2/opencv.hpp>
#include "OpenCLManager.h"
class ProcessingImage {
 private:
   cv::Mat image;
   OpenCLManager openCLManager;
   cl::size_t<3> origin;
   cl::size_t<3> region;
   cl::NDRange localRange;

 public:
   void Threshold(const float threshold = 0.5);
   void Dilate();
   void Erode();
   cv::Mat GetImage();
   void SetImageToProcess(cv::Mat img);
   ~ProcessingImage();
   ProcessingImage(cv::Mat, OpenCLManager openCLManager);
};

#endif // IMAGE_H