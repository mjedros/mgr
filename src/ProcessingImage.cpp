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
   localRange = {32, 16};
}

void ProcessingImage::Dilate()
{
   cl::Kernel kernel = cl::Kernel(openCLManager->program, "Dilate");
   cl::ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
   cl::Image2D image_in(openCLManager->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                        image.cols, image.rows, 0, image.data);
   cl::Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, format, image.cols, image.rows);
   cl_float3 ellipseparams = (cl_float3) {{0.3, 1.5, 2}};
   kernel.setArg(0, image_in);
   kernel.setArg(1, image_out);
   kernel.setArg(2, ellipseparams);
   Process(kernel, image_out);
}

void ProcessingImage::Erode()
{
   cl::Kernel kernel = cl::Kernel(openCLManager->program, "Erode");
   cl::ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
   cl::Image2D image_in(openCLManager->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                        image.cols, image.rows, 0, image.data);
   cl::Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, format, image.cols, image.rows);
   cl_float3 ellipseparams = (cl_float3) {{0, 2, 2}};
   kernel.setArg(0, image_in);
   kernel.setArg(1, image_out);
   kernel.setArg(2, ellipseparams);
   Process(kernel, image_out);
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
