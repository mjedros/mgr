#include "ApplicationManager.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace CLProcessingImage;
ApplicationManager::ApplicationManager(const std::shared_ptr<OpenCLManager> &openCLManagerPtr)
    : openCLManager(openCLManagerPtr)
{
}

void ApplicationManager::DoSth()
{

   std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
   std::unique_ptr<IImageSource> imageSource =
       SourceFactory::GetImageSource(VideoFile, "../Data/768x576.avi");
   imageSource->Start();
   for (Mat im = imageSource->Get(); !im.empty(); im = imageSource->Get())
   {
      Mat imageIn;

      cvtColor(im, imageIn, CV_BGR2GRAY);
      img->SetStructuralElement(CLProcessingImage::RECTANGLE,{2,2});
      img->SetImageToProcess(imageIn.clone());
      img->Threshold(0.35);
      img->Dilate();
      img->Erode();
      img->SetStructuralElement(CLProcessingImage::CROSS,{1,1});
      img->Skeletonize();
      imshow("Processed", img->GetImage());
      img->SetImageToProcess(imageIn);
      img->Threshold(0.35);
      img->Contour();
      imshow("Contour", img->GetImage());
      //imshow("normal", im);
      int key = cv::waitKey(1);
      if (key != -1)
       break;
   }
}
