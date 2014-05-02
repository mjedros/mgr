#include "ApplicationManager.h"
#include <opencv2/opencv.hpp>
using namespace cv;
ApplicationManager::ApplicationManager(const std::shared_ptr<OpenCLManager> &openCLManagerPtr)
    : openCLManager(openCLManagerPtr)
{
}

void ApplicationManager::DoSth()
{

   std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
   std::unique_ptr<IImageSource> imageSource =
       SourceFactory::GetImageSource(VideoFile, "../Data/napis.jpg");
   imageSource->Start();
   for (Mat im = imageSource->Get(); !im.empty(); im = imageSource->Get())
   {
      Mat imageIn;

      cvtColor(im, imageIn, CV_BGR2GRAY);
      img->SetImageToProcess(imageIn.clone());
      img->Threshold(0.35);
      // img->Dilate();
      //  img->Erode();
      img->Skeletonize();
      imshow("Processed", img->GetImage());
      img->SetImageToProcess(imageIn);
      img->Threshold(0.35);
      img->Contour();
      imshow("Contour", img->GetImage());
      imshow("normal", im);
      cv::waitKey(0);
      // if (key != -1)
      // break;
   }
}
