#include "ProcessingImage.h"
using namespace cv;
ProcessingImage::ProcessingImage(const std::shared_ptr<OpenCLManager> &openCLManagerPtr)
    : openCLManager(openCLManagerPtr)
{
   origin[0] = origin[1] = origin[2] = 0;
}

ProcessingImage::~ProcessingImage() { image.release(); }

Mat ProcessingImage::GetImage() { return image; }
void ProcessingImage::SetImageToProcess(cv::Mat img)
{
   image.release();
   image = img;
   region[0] = image.cols;
   region[1] = image.rows;
   region[2] = 1;
   localRange = {16, 16};
}

void ProcessingImage::Dilate()
{
   cl::Kernel kernel = cl::Kernel(openCLManager->program, "DilateCross");
   cl::ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
   cl::Image2D image_in(openCLManager->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                        image.cols, image.rows, 0, image.data);
   cl::Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, format, image.cols, image.rows);
   kernel.setArg(0, image_in);
   kernel.setArg(1, image_out);
   SetStructuralElementArgument(kernel,structuralElementType,structuralElementParams);
   Process(kernel, image_out);
}

void ProcessingImage::Erode()
{

   cl::Kernel kernel = cl::Kernel(openCLManager->program, "ErodeCross");
   cl::ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
   cl::Image2D image_in(openCLManager->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                        image.cols, image.rows, 0, image.data);
   cl::Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, format, image.cols, image.rows);
   kernel.setArg(0, image_in);
   kernel.setArg(1, image_out);
   SetStructuralElementArgument(kernel,structuralElementType,structuralElementParams);
   Process(kernel, image_out);
}

void ProcessingImage::Contour()
{
   Erode();
   cv::Mat temp = image.clone();
   Dilate();
   image -= temp;
}

void ProcessingImage::Skeletonize()
{
   cv::Mat skel(image.size(), CV_8U, cv::Scalar(0));
   cv::Mat eroded;
   cv::Mat img;
   bool done;
   do
   {
      image.copyTo(img);
      Erode();
      eroded = image.clone();
      Dilate();
      img -= image;
      done = (cv::countNonZero(image) < 100);
      cv::bitwise_or(skel, img, skel);

      image = eroded;
   } while (!done);
   image = skel;
}

void ProcessingImage::SetStructuralElement(StructuralElement element, std::vector<float> params)
{
    structuralElementType = element;
    structuralElementParams = params;
}

void ProcessingImage::SetStructuralElementArgument(cl::Kernel &kernel, StructuralElement element, std::vector<float> params)
{
    switch(element)
    {
    case cross:
    {
        cl_int2 rectParams = (cl_int2) {{(int)params[0], (int)params[1]}};
        kernel.setArg(2, rectParams);
    }
   default:
     break;
    }
}

void ProcessingImage::Threshold(const float threshold)
{
   cl::Kernel kernel = cl::Kernel(openCLManager->program, "threshold");

   cl::ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
   cl::Image2D image_in(openCLManager->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                        image.cols, image.rows, 0, image.data);
   cl::Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, format, image.cols, image.rows);
   kernel.setArg(0, image_in);
   kernel.setArg(1, image_out);
   kernel.setArg(2, threshold);
   Process(kernel, image_out);
}

void ProcessingImage::Process(cl::Kernel &kernel, cl::Image2D &image_out)
{
   openCLManager->queue.enqueueNDRangeKernel(
       kernel, cl::NullRange, cl::NDRange(image.cols, image.rows), localRange, NULL, NULL);
   openCLManager->queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0, 0, image.data);
}
