#ifndef QT_NOT_DEFINED
#include "../Gui/mainwindow.h"
#include <QApplication>
#endif
#include <iostream>
#include "OpenCLManager.h"
#include "ProcessingImage.h"
#include <opencv2/opencv.hpp>
#include "SourceFactory.h"
using namespace cl;
using namespace cv;

#ifndef QT_NOT_DEFINED
int main(int argc, char *argv[]) {
   QApplication a(argc, argv);
   MainWindow w;
   w.show();

   return a.exec();
}
#else
int main(/*int argc, char *argv[]*/) {
   OpenCLManager openCLManager;
   auto imageSource = std::unique_ptr<IImageSource>(new FileVideo("../Data/768x576.avi"));
   imageSource->Start();
   try {
      openCLManager.Configure("../Kernels/Kernels.cl", 0, 0);
      std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
      for (Mat im = imageSource->Get(); !im.empty(); im = imageSource->Get()) {
          Mat imageIn;
          imshow("normal",im);
          cvtColor(im, imageIn, CV_BGR2GRAY);
            img->SetImageToProcess(imageIn);
            img->Threshold(0.45);
            img->Erode();
            img->Dilate();
            imshow("Processed",imageIn);
            int key = cv::waitKey(1);
            if(key != -1)
                break;

      }

   }
   catch (Error &e) {
      std::cout << e.what() << " error, number= " << e.err() << std::endl;
   }
}
#endif
