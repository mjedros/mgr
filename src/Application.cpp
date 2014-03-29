#ifndef QT_NOT_DEFINED
#include "../Gui/mainwindow.h"
#include <QApplication>
#endif
#include <iostream>
#include "OpenCLManager.h"
#include <opencv2/opencv.hpp>
using namespace cl;
using namespace cv;

std::vector<Mat> Dilate(OpenCLManager &openCLManager, Mat &imageIn) {
   std::vector<Mat> answerMat;

   cl::Kernel kernel = cl::Kernel(openCLManager.program, "Dilate");
   const int width = imageIn.cols;
   const int height = imageIn.rows;
   cl::ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
   cl::Image2D image_in(openCLManager.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                        width, height, 0, imageIn.data);
   cl::Image2D image_out(openCLManager.context, CL_MEM_WRITE_ONLY, format, width, height);
   kernel.setArg(0, image_in);
   kernel.setArg(1, image_out);
   openCLManager.queue.enqueueNDRangeKernel(kernel, cl::NullRange,
                                            cl::NDRange(imageIn.cols, imageIn.rows),
                                            cl::NDRange(16, 16), NULL, NULL);

   cl::size_t<3> origin;
   origin[0] = origin[1] = origin[2] = 0;
   cl::size_t<3> region;
   region[0] = imageIn.cols;
   region[1] = imageIn.rows;
   region[2] = 1;

   answerMat.emplace_back(Size(imageIn.cols, imageIn.rows), 0);
   openCLManager.queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0, 0,
                                        answerMat[0].data);

   cv::waitKey(1000);
   return (answerMat);
}

std::vector<Mat> Dosomething(OpenCLManager &openCLManager) {
   std::vector<Mat> answerMat;

   cl::Kernel kernel = cl::Kernel(openCLManager.program, "sobel_rgb");

   Mat ColorImage = imread("../lena.jpg");

   Mat imageIn;
   cvtColor(ColorImage, imageIn, CV_BGR2GRAY);
   const int width = imageIn.cols;
   const int height = imageIn.rows;
   cl::ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
   cl::ImageFormat format2(CL_LUMINANCE, CL_UNORM_INT8);
   cl::Image2D image_in(openCLManager.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                        width, height, 0, imageIn.data);
   cl::Image2D image_out(openCLManager.context, CL_MEM_WRITE_ONLY, format2, width, height);

   kernel.setArg(0, image_in);
   kernel.setArg(1, image_out);
   openCLManager.queue.enqueueNDRangeKernel(kernel, cl::NullRange,
                                            cl::NDRange(imageIn.cols, imageIn.rows),
                                            cl::NDRange(1, 1), NULL, NULL);

   cl::size_t<3> origin;
   origin[0] = origin[1] = origin[2] = 0;
   cl::size_t<3> region;
   region[0] = imageIn.cols;
   region[1] = imageIn.rows;
   region[2] = 1;

   answerMat.emplace_back(Size(imageIn.cols, imageIn.rows), 0);
   openCLManager.queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0, 0,
                                        answerMat[0].data);

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
   try {
      openCLManager.Configure("../Kernels.cl", 0, 0);
      const auto ans = Dosomething(openCLManager);
      Mat a = ans[0];
      for (const auto &item : ans)
         imshow("", item);
      auto pic = Dilate(openCLManager, a);
      imshow("2", pic[0]);
   }
   catch (Error &e) {
      std::cout << e.what() << " error, number= " << e.err() << std::endl;
   }
   cv::waitKey(0);
}
#endif
