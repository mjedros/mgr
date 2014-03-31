#ifndef QT_NOT_DEFINED
#include "../Gui/mainwindow.h"
#include <QApplication>
#endif
#include <iostream>
#include "OpenCLManager.h"
#include "ProcessingImage.h"
#include <opencv2/opencv.hpp>
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
   try {
      openCLManager.Configure("../../Kernels/Kernels.cl", 0, 0);
      Mat ColorImage = imread("../background.png");
      Mat imageIn;
      cvtColor(ColorImage, imageIn, CV_BGR2GRAY);
      std::unique_ptr<ProcessingImage> img(new ProcessingImage(imageIn, openCLManager));

      imshow("normal", img->GetImage());
      img->Threshold(0.35f);
      imshow("threshold", img->GetImage());
      for (int i = 0; i < 2; i++) {

         img->Dilate();
         imshow("dilated", img->GetImage());
         cv::waitKey(1);
      }
      img->Erode();
      imshow("erode", img->GetImage());
      Mat otherimg = imread("../lena.jpg");
      Mat othergray;

      cvtColor(otherimg, otherimg, CV_BGR2GRAY);
      img->SetImageToProcess(otherimg);
      std::cout << "done" << std::endl;
      img->Threshold(0.35f);
      img->Dilate();
      img->Dilate();
      img->Erode();
      imshow("other", img->GetImage());
      ;
   }
   catch (Error &e) {
      std::cout << e.what() << " error, number= " << e.err() << std::endl;
   }
   cv::waitKey(0);
}
#endif
