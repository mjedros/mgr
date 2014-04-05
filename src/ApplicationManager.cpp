#include "ApplicationManager.h"
#include <opencv2/opencv.hpp>
using namespace cv;
ApplicationManager::ApplicationManager(OpenCLManager &openCLManager)
{
 openCLManager = openCLManager;
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
       imshow("normal", im);
       cvtColor(im, imageIn, CV_BGR2GRAY);
       img->SetImageToProcess(imageIn);
       img->Threshold(0.45);
       img->Erode();
       img->Dilate();
       imshow("Processed", imageIn);
       int key = cv::waitKey(1);
       if (key != -1)
          break;
    }
}
