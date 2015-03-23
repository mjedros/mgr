#include "ProcessingImage.h"
#include <chrono>

using namespace cv;
using namespace cl;
namespace Mgr {
std::map<std::string, StructuralElement> StrElementMap = {
  { "Ellipse", ELLIPSE }, { "Cross", CROSS }, { "Rectangle", RECTANGLE }
};
unsigned int counter = 0;
unsigned int timeSum = 0;

void
ProcessingImage::performMorphologicalOperation(const std::string &Operation) {
  const std::string kernelName = Operation + structuralElementType;
  Kernel kernel(openCLManager->program, kernelName.c_str());
  const ImageFormat format(CL_R, CL_UNORM_INT8);
  Image2D image_in(openCLManager->context,
                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format, image.cols,
                   image.rows, 0, image.data);
  Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, format,
                    image.cols, image.rows);
  setStructuralElementArgument(kernel);
  process(kernel, image_in, image_out);
}

ProcessingImage::ProcessingImage(
    const std::shared_ptr<OpenCLManager> &openCLManagerPtr)
  : image(cv::Mat()), openCLManager(std::move(openCLManagerPtr)) {
  origin[0] = origin[1] = origin[2] = 0;
}

Mat ProcessingImage::getImage() { return image; }
void ProcessingImage::setImageToProcess(const cv::Mat &img) {
  image.release();
  image = img;
  region[0] = img.cols;
  region[1] = img.rows;
  region[2] = 1;
}

void ProcessingImage::dilate() { performMorphologicalOperation("Dilate"); }

void ProcessingImage::erode() { performMorphologicalOperation("Erode"); }

void ProcessingImage::contour() {
  erode();
  cv::Mat temp = image.clone();
  dilate();
  image -= temp;
}

void ProcessingImage::skeletonize() {
  cv::Mat skel(image.size(), CV_8UC1, cv::Scalar(0));
  cv::Mat eroded;
  do {
    cv::Mat img = image.clone();
    erode();
    eroded = image.clone();
    dilate();
    img -= image;

    cv::bitwise_or(skel, img, skel);
    image = eroded;
  } while (!(cv::countNonZero(eroded) == 0));
  image = skel;
}

void ProcessingImage::setStructuralElement(const std::string &element,
                                           const std::vector<float> &params) {
  if (StrElementMap.find(element.c_str()) == StrElementMap.end()) {
    throw std::string("Not existing structural element");
  }

  structuralElementType = element;
  structuralElementParams = params;
}

void ProcessingImage::setStructuralElementArgument(cl::Kernel &kernel) {
  switch (StrElementMap[structuralElementType.c_str()]) {
  case ELLIPSE: {
    const cl_float3 ellipseParams =
        (cl_float3){ { structuralElementParams[0], structuralElementParams[1],
                       structuralElementParams[2] } };
    kernel.setArg(2, ellipseParams);
    break;
  }
  case RECTANGLE:
  case CROSS: {
    const cl_int2 rectParams = (cl_int2){ {(int)structuralElementParams[0],
                                           (int)structuralElementParams[1] } };
    kernel.setArg(2, rectParams);
    break;
  }
  default:
    break;
  }
}

void ProcessingImage::binarize(const unsigned int &minimum,
                               const unsigned int &maximum) {
  cl::Kernel kernel = cl::Kernel(openCLManager->program, "Binarize");

  const cl::ImageFormat format(CL_A, CL_UNSIGNED_INT8);
  const cl::ImageFormat formatOut(CL_R, CL_UNORM_INT8);

  try {
    cl::Image2D image_in(openCLManager->context, CL_MEM_READ_ONLY, format,
                         image.cols, image.rows, 0);
    cl::Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, formatOut,
                          image.cols, image.rows, 0);
    openCLManager->queue.enqueueWriteImage(image_in, CL_TRUE, origin, region, 0,
                                           0, image.data);
    kernel.setArg(2, minimum);
    kernel.setArg(3, maximum);
    process(kernel, image_in, image_out);
  } catch (Error &e) {
    LOG(e.what());
    LOG(e.err());
  } catch (...) {
    LOG("Unknown Error");
  }
}

void ProcessingImage::process(cl::Kernel &kernel, cl::Image2D &image_in,
                              cl::Image2D &image_out) {
  kernel.setArg(0, image_in);
  kernel.setArg(1, image_out);
  counter++;
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  openCLManager->queue.enqueueNDRangeKernel(kernel, cl::NDRange(0, 0),
                                            cl::NDRange(image.cols, image.rows),
                                            cl::NullRange, NULL, NULL);
  openCLManager->queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0,
                                        0, image.data);
  end = std::chrono::system_clock::now();
  timeSum += std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                 .count();
}

int getAvarage() { return timeSum / counter; }
void clear() { timeSum = counter = 0; }
}
