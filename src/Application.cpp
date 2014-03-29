#ifndef QT_NOT_DEFINED
#include "../Gui/mainwindow.h"
#include <QApplication>
#endif
#include <iostream>
#include "OpenCLManager.h"
#include <opencv2/opencv.hpp>
using namespace cl;
using namespace cv;
std::vector<Mat> Dosomething(OpenCLManager &openCLManager) {

  cl::Kernel kernel = cl::Kernel(openCLManager.program, "sobel_rgb");
;
  std::vector<Mat> answerMat;
  Mat ColorImage = imread("../background.png");

  Mat imageIn;
  cvtColor(ColorImage, imageIn, CV_BGR2GRAY);
  const int width = imageIn.cols;
  const int height = imageIn.rows;
  cl::ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
  cl::Image2D image_in(openCLManager.context,
                       CL_MEM_READ_ONLY,
                       format, width,
                       height, 0, imageIn.data);
  cl::Image2D image_out(openCLManager.context, CL_MEM_WRITE_ONLY,
                        format, width,
                        height);

  kernel.setArg(0, image_in);
  kernel.setArg(1, image_out);

  try {
    openCLManager.queue.enqueueNDRangeKernel(
        kernel, cl::NullRange, cl::NDRange(imageIn.cols, imageIn.rows),
        cl::NDRange(4, 4), NULL, NULL);
  }
  catch (cl::Error &e) {
    std::cout << "enque error" << e.err() << std::endl;
  }
  cl::size_t<3> origin;
  origin[0] = 0;
  origin[1] = 0;
  origin[2] = 0;
  cl::size_t<3> region;
  region[0] = imageIn.cols;
  region[1] = imageIn.rows;
  region[2] = 1;
  answerMat.emplace_back(Size(imageIn.cols, imageIn.rows), CV_8U);
  try {

    openCLManager.queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0,
                                         0, answerMat[0].data);
  }
  catch (cl::Error &e) {
    std::cout << e.err() << std::endl;
  }
  cv::waitKey(1000);
  return (answerMat);
}

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
  openCLManager.Configure("../Kernels.cl", 0, 0);
  const auto ans = Dosomething(openCLManager);
  for (const auto &item : ans)
    imshow("", item);

  cv::waitKey(1000);
}
#endif
