#include "ApplicationManager.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace CLProcessingImage;
ApplicationManager::ApplicationManager(const std::shared_ptr<OpenCLManager> &openCLManagerPtr)
    : openCLManager(openCLManagerPtr)
{
}

void ApplicationManager::SetFileToProcess(std::string filename) { sourceFilename = filename; }

void ApplicationManager::DoSth()
{
   cvImageWindow win;
   win.show();

   std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
   std::unique_ptr<IImageSource> imageSource =
       SourceFactory::GetImageSource(VideoFile, sourceFilename);
   imageSource->Start();
   for (Mat im = imageSource->Get(); !im.empty(); im = imageSource->Get())
   {
      Mat imageIn;

      cvtColor(im, imageIn, CV_BGR2GRAY);
      img->SetImageToProcess(imageIn.clone());
      img->Threshold(0.35);
      img->SetStructuralElement(CROSS, {1, 1});
      img->Skeletonize();
      win.draw(img->GetImage());
      img->Threshold(0.35);
      img->Contour();

      // imshow("s",imageIn);
      // imshow("normal", im);
      cv::waitKey(1);
      if (win.closed)
      {
         break;
      }
   }
   while (!win.closed)
   {
      cv::waitKey(1);
   }
}
