#include "ProcessingImage.h"

#include "Logger.h"
#include "Image3d.h"
#include <chrono>
#include <vector>

using namespace cv;
using namespace cl;
namespace Mgr {

static Logger &logger = Logger::getInstance();
std::map<std::string, OPERATION> ProcessingImage::OperationMap = {
  { "Dilation", OPERATION::DILATION },
  { "Erosion", OPERATION::EROSION },
  { "Contour", OPERATION::CONTOUR },
  { "Opening", OPERATION::OPENING },
  { "Closing", OPERATION::CLOSING },
  { "Skeletonize", OPERATION::SKELETONIZATION },
  { "Skeletonize2", OPERATION::SKELETONIZATION2 }
};
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
namespace {
int getShape(const std::string &structuralElementType) {
  if (structuralElementType == "Rectangle")
    return cv::MORPH_RECT;
  else if (structuralElementType == "Cross")
    return cv::MORPH_CROSS;
  return cv::MORPH_ELLIPSE;
}
}
static const ImageFormat format(CL_R, CL_UNORM_INT8);
void ProcessingImage::performMorphologicalOperation() {
  if (newImage)
    getROIOOutOfMat();
  if (processOpenCV) {
    logger.beginOperation();
    cv::Mat element = cv::getStructuringElement(
        getShape(structuralElementType),
        cv::Size(structuralElementParams.at(0) * 2 + 1,
                 structuralElementParams.at(1) * 2 + 1));
    if (kernelOperation == "Dilate")
      cv::dilate(*imageToProcess, *imageToProcess, element);
    else
      cv::erode(*imageToProcess, *imageToProcess, element);
    logger.endOperation();
    updateFullImage();
    return;
  }
  if (newImage) {
    Image2D image_in(
        openCLManager.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
        imageToProcess->cols, imageToProcess->rows, 0, imageToProcess->data);
    Image2D image_out(openCLManager.context, CL_MEM_READ_WRITE, format,
                      imageToProcess->cols, imageToProcess->rows);
    process(kernel, image_in, image_out);
    updateFullImage();
  } else {
    if (!image_in_ptr) {
      getROIOOutOfMat();
      image_in_ptr = std::unique_ptr<Image2D>(
          new Image2D(openCLManager.context, CL_MEM_READ_ONLY, format,
                      imageToProcess->cols, imageToProcess->rows));
      openCLManager.queue.enqueueWriteImage(*image_in_ptr, CL_TRUE, origin,
                                            region, 0, 0, imageToProcess->data);
    } else {
      image_in_ptr.reset(image_out_ptr.release());
    }
    image_out_ptr = std::unique_ptr<Image2D>(
        new Image2D(openCLManager.context, CL_MEM_READ_WRITE, format,
                    imageToProcess->cols, imageToProcess->rows));

    process(kernel, *image_in_ptr, *image_out_ptr);
  }
}

ProcessingImage::ProcessingImage(OpenCLManager &openCLManagerRef,
                                 bool processRoi)
  : image(cv::Mat()), openCLManager(openCLManagerRef), processROI(processRoi) {
  origin[0] = origin[1] = origin[2] = 0;
  strElementMap = { { "Ellipse", ELLIPSE },
                    { "Cross", CROSS },
                    { "Rectangle", RECTANGLE },
                    { "EllipseImage", ELLIPSEIMG } };
}
Mat ProcessingImage::getImage() const { return image; }

void ProcessingImage::substr() {
  kernel = cl::Kernel(openCLManager.program, "Substract");
  std::unique_ptr<Image2D> image_in2_ptr =
      std::unique_ptr<Image2D>(image_out_ptr.release());
  image_out_ptr = std::unique_ptr<Image2D>(
      new Image2D(openCLManager.context, CL_MEM_READ_WRITE, format,
                  imageToProcess->cols, imageToProcess->rows));
  kernel.setArg(2, *image_in2_ptr);
  process(kernel, *image_in_ptr, *image_out_ptr);
}

void ProcessingImage::readImage() {
  if (!processingSequence && !processOpenCV) {
    openCLManager.queue.enqueueReadImage(*image_out_ptr, CL_TRUE, origin,
                                         region, 0, 0, imageToProcess->data);
    updateFullImage();
    image_in_ptr.reset();
    newImage = true;
  }
}

void ProcessingImage::processSequence(OperationsVector operVect) {
  processingSequence = true;
  getROIOOutOfMat();
  for (auto &tokens : operVect) {
    newImage = false;
    setStructuralElement(
        tokens[1],
        { std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4]) });
    if (tokens[0] == "Closing")
      close();
    else if (tokens[0] == "Dilation")
      dilate();
    else if (tokens[0] == "Erosion")
      erode();
    else if (tokens[0] == "Skeletonize")
      skeletonize();
    else if (tokens[0] == "Skeletonize2")
      skeletonize2();
    else if (tokens[0] == "Opening")
      open();
    else if (tokens[0] == "Contour")
     contour();
  }
  processingSequence = false;
  readImage();
}
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
  if (processOpenCV) {
    cv::Mat temp = image.clone();
    erode();
    image = temp - image;
    return;
  }
  newImage = false;
  erode();
  // dilate();
  substr();
  readImage();
}

void ProcessingImage::open() {
  newImage = false;
  erode();
  dilate();
  readImage();
}

void ProcessingImage::close() {
  newImage = false;
  dilate();
  erode();
  readImage();
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
  } while (cv::countNonZero(eroded) != 0.0);
  image = skel;
}

void ProcessingImage::skeletonize2() {
  kernel = cl::Kernel(openCLManager.program, "Skeletonize");
  cv::Mat img;
  newImage = false;
  do {
    img = image.clone();
    for (int i = 0; i < 4; ++i) {
      kernel.setArg(2, i);
      performMorphologicalOperation();
    }
    openCLManager.queue.enqueueReadImage(*image_out_ptr, CL_TRUE, origin,
                                         region, 0, 0, imageToProcess->data);
    updateFullImage();
    image_in_ptr.reset();
  } while (cv::countNonZero(img != image) != 0);
  if (!processingSequence)
    newImage = true;
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
    cl::Image2D image_in(openCLManager.context, CL_MEM_READ_WRITE, format,
                         imageToProcess->cols, imageToProcess->rows, 0);
    cl::Image2D image_out(openCLManager.context, CL_MEM_READ_WRITE, formatOut,
                          imageToProcess->cols, imageToProcess->rows, 0);

    openCLManager.queue.enqueueWriteImage(image_in, CL_TRUE, origin, region, 0,
                                          0, imageToProcess->data);

    kernel.setArg(2, minimum);
    kernel.setArg(3, maximum);
    process(kernel, image_in, image_out);
    openCLManager.queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0,
                                         0, imageToProcess->data);
    updateFullImage();
  } catch (Error &e) {
    logger.printError(e);
  } catch (...) {
    logger.printLine("Unknown Error");
  }
}
#include <ctime>
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
  if (newImage)
    openCLManager.queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0,
                                         0, imageToProcess->data);
  event.wait();
  const auto elapsed = event.getProfilingInfo<CL_PROFILING_COMMAND_END>() -
                       event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  logger.endOperation(elapsed);
}
void ProcessingImage::getROIOOutOfMat() {
  if (!processROI)
    imageToProcess = &image;
  else {
    roiImage = Mat(image, Rect(roi.first.first, roi.second.first,
                               roi.first.second - roi.first.first,
                               roi.second.second - roi.second.first)).clone();
    imageToProcess = &roiImage;
  }

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
