#include "ProcessingImage.h"

#include "Logger.h"
#include "Image3d.h"
#include <chrono>
#include <vector>

using namespace cv;
using namespace cl;
namespace Mgr {

static Logger &logger = Logger::getInstance();

void ProcessingImage::setKernel(const std::string &Operation) {
  setKernelOperation(Operation);
}

void ProcessingImage::setKernelWithOperation(const OPERATION &Operation) {
  switch (Operation) {
  case OPERATION::DILATION:
    setKernel("Dilate");
    break;
  case OPERATION::EROSION:
    setKernel("Erode");
    break;
  default:
    return;
  }
}

void ProcessingImage::performMorphologicalOperation() {
  getROIOOutOfMat();
  const ImageFormat format(CL_R, CL_UNORM_INT8);
  Image2D image_in(
      openCLManager.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
      imageToProcess->cols, imageToProcess->rows, 0, imageToProcess->data);
  Image2D image_out(openCLManager.context, CL_MEM_WRITE_ONLY, format,
                    imageToProcess->cols, imageToProcess->rows);
  process(kernel, image_in, image_out);
  updateFullImage();
}

ProcessingImage::ProcessingImage(OpenCLManager &openCLManagerRef,
                                 bool processRoi)
  : image(cv::Mat()), imageToProcess(nullptr), openCLManager(openCLManagerRef),
    processROI(processRoi) {
  origin[0] = origin[1] = origin[2] = 0;
  strElementMap = { { "Ellipse", ELLIPSE },
                    { "Cross", CROSS },
                    { "Rectangle", RECTANGLE },
                    { "EllipseImage", ELLIPSEIMG } };
}

Mat ProcessingImage::getImage() const { return image; }
void ProcessingImage::setImageToProcess(const cv::Mat &img) {
  image.release();
  image = img;
  region[0] = img.cols;
  region[1] = img.rows;
  region[2] = 1;
}

void ProcessingImage::dilate() {
  setKernel("Dilate");
  performMorphologicalOperation();
}

void ProcessingImage::erode() {
  setKernel("Erode");
  performMorphologicalOperation();
}

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
  if (strElementMap.find(element.c_str()) == strElementMap.end()) {
    throw std::string("Not existing structural element");
  }

  structuralElementType = element;
  structuralElementParams = params;
}

void ProcessingImage::setStructuralElementArgument() {
  switch (strElementMap[structuralElementType.c_str()]) {
  case ELLIPSE: {
    const cl_float3 ellipseParams = { { structuralElementParams[0],
                                        structuralElementParams[1],
                                        structuralElementParams[2] } };
    kernel.setArg(2, ellipseParams);
    break;
  }
  case RECTANGLE:
  case CROSS: {
    const cl_int2 rectParams = { {(int)structuralElementParams[0],
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
  cl::Kernel kernel = cl::Kernel(openCLManager.program, "Binarize");
  const cl::ImageFormat format(CL_A, CL_UNSIGNED_INT8);
  const cl::ImageFormat formatOut(CL_R, CL_UNORM_INT8);

  try {
    getROIOOutOfMat();
    cl::Image2D image_in(openCLManager.context, CL_MEM_READ_ONLY, format,
                         imageToProcess->cols, imageToProcess->rows, 0);
    cl::Image2D image_out(openCLManager.context, CL_MEM_WRITE_ONLY, formatOut,
                          imageToProcess->cols, imageToProcess->rows, 0);
    openCLManager.queue.enqueueWriteImage(image_in, CL_TRUE, origin, region, 0,
                                          0, imageToProcess->data);

    kernel.setArg(2, minimum);
    kernel.setArg(3, maximum);
    process(kernel, image_in, image_out);
    updateFullImage();
  } catch (Error &e) {
    logger.printError(e);
  } catch (...) {
    logger.printLine("Unknown Error");
  }
}

void ProcessingImage::process(cl::Kernel &kernel, cl::Image2D &image_in,
                              cl::Image2D &image_out) {
  kernel.setArg(0, image_in);
  kernel.setArg(1, image_out);
  logger.beginOperation();
  cl::Event event;
  openCLManager.queue.enqueueNDRangeKernel(
      kernel, cl::NDRange(0, 0),
      cl::NDRange(imageToProcess->cols, imageToProcess->rows), cl::NullRange,
      NULL, &event);
  event.wait();

  openCLManager.queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0, 0,
                                       imageToProcess->data);

  const auto elapsed = event.getProfilingInfo<CL_PROFILING_COMMAND_END>() -
                       event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  logger.endOperation(elapsed);
}
void ProcessingImage::getROIOOutOfMat() {
  //  static int number = 0;
  imageToProcess.release();
  if (!processROI)
    imageToProcess = std::unique_ptr<Mat>(&image);
  else {
    roiImage = Mat(image, Rect(roi.first.first, roi.second.first,
                               roi.first.second - roi.first.first,
                               roi.second.second - roi.second.first)).clone();
    imageToProcess = std::unique_ptr<Mat>(&roiImage);
  }
  //  cv::imwrite("/home/michal/otherdata/pic" + std::to_string(number++) +
  //  ".jpg",
  //              *imageToProcess);
  region[0] = imageToProcess->cols;
  region[1] = imageToProcess->rows;
}

void ProcessingImage::updateFullImage() {
  if (!processROI) {
    image = *imageToProcess;
    return;
  }
  image = Mat(image.rows, image.cols, imageToProcess->type(), 0.0);
  imageToProcess->copyTo(
      image(Rect(roi.first.first, roi.second.first, imageToProcess->cols,
                 imageToProcess->rows)));
}
}
