#include "ProcessingImage3d.h"

#include "Image3d.h"
#include "Logger.h"
#include "OpenCLManager.h"

using namespace cl;
using namespace cv;

namespace Mgr {
static Logger &logger = Logger::getInstance();
namespace {
cv::Mat getEllipse(const float xRadius, const float yRadius, const int rows,
                   const int cols, const int type) {
  cv::Mat ellipse(rows, cols, type, cv::Scalar(0, 0, 0));
  if ((int)xRadius == 0) {
    ellipse.at<uchar>(rows / 2, cols / 2) = 255;
    return ellipse;
  }
  for (int i = -xRadius; i <= xRadius; ++i) {
    for (int j = -yRadius; j <= yRadius; ++j) {
      if (((i * i) / (xRadius * xRadius) + (j * j) / (yRadius * yRadius) <= 1))
        ellipse.at<uchar>(rows / 2 + i, cols / 2 + j) = 255;
    }
  }
  return ellipse;
}
cv::Mat getEllipsoidImage(std::vector<float> params, cv::Mat img) {
  Image3d ellipse3d(params[2] * 2 + 1,
                    cv::Mat(params[1] * 2 + 1, params[0] * 2 + 1, img.type(),
                            cv::Scalar(0, 0, 0)));
  const float radius = params[2];
  for (float i = 0; i <= radius; i++) {
    const float param =
        sqrt(1.0 - (radius - i) * (radius - i) / (radius * radius));
    float xRadius = (float)params[1] * param;
    ;
    float yRadius = (float)params[0] * param;

    auto ellipse = getEllipse(xRadius, yRadius, ellipse3d.getRows(),
                              ellipse3d.getCols(), img.type());
    ellipse3d.setImageAtDepth(i, ellipse);
    ellipse3d.setImageAtDepth(ellipse3d.getDepth() - i - 1, ellipse);
  }
  return ellipse3d.get3dMatImage();
}
}

ProcessingImage3d::ProcessingImage3d(OpenCLManager &openCLManagerRef,
                                     bool processRoi)
  : ProcessingImage(openCLManagerRef, processRoi) {}

void ProcessingImage3d::set3dImageToProcess(const Image3d &image3dToProcess) {
  image.release();
  image = image3dToProcess.get3dMatImage().clone();
  region[0] = image3dToProcess.getCols();
  region[1] = image3dToProcess.getRows();
  region[2] = image3dToProcess.getDepth();
  imageToProcess.release();
  image3d = image3dToProcess;
}

void ProcessingImage3d::setKernel(const std::string &Operation) {
  setKernelOperation(Operation + "3d");
}

void ProcessingImage3d::setStructuralElementArgument() {
  switch (strElementMap[structuralElementType.c_str()]) {
  case StructuralElement::ELLIPSE: {
    const cl_float3 ellipseParams = { { structuralElementParams[0],
                                        structuralElementParams[1],
                                        structuralElementParams[2] } };
    kernel.setArg(2, ellipseParams);
    break;
  }
  case StructuralElement::RECTANGLE:
  case StructuralElement::CROSS: {
    const cl_int3 params = { {(int)structuralElementParams[0],
                              (int)structuralElementParams[1],
                              (int)structuralElementParams[2] } };
    kernel.setArg(2, params);
    break;
  }
  case StructuralElement::ELLIPSEIMG: {
    ellipsoidImage = getEllipsoidImage(structuralElementParams, image).clone();
    const ImageFormat format(CL_R, CL_UNORM_INT8);
    ellipseIn3d = std::unique_ptr<cl::Image3D>(new cl::Image3D(
        openCLManager.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
        structuralElementParams[0] * 2 + 1, structuralElementParams[1] * 2 + 1,
        structuralElementParams[2] * 2 + 1, 0, 0, ellipsoidImage.data));
    const cl_float3 ellipseParams = { { structuralElementParams[0],
                                        structuralElementParams[1],
                                        structuralElementParams[2] } };
    kernel.setArg(2, *ellipseIn3d);
    kernel.setArg(3, ellipseParams);
    break;
  }
  default:
    break;
  }
}

void ProcessingImage3d::performOperation(Image3D &image_out3d) {
  cl::Event event;
  try {
    openCLManager.queue.enqueueNDRangeKernel(
        kernel, cl::NDRange(0, 0), cl::NDRange(region[0], region[1], region[2]),
        cl::NullRange, NULL, &event);

    openCLManager.queue.enqueueReadImage(image_out3d, CL_TRUE, origin, region,
                                         0, 0, imageToProcess->data);
    event.wait();
    const auto elapsed = event.getProfilingInfo<CL_PROFILING_COMMAND_END>() -
                         event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    logger.endOperation(elapsed);
  } catch (cl::Error &e) {
    logger.printError(e);
  }
}

void ProcessingImage3d::performMorphologicalOperation() {
  getROIOOutOfMat();
  const ImageFormat format(CL_R, CL_UNORM_INT8);
  logger.beginOperation();
  cl::Image3D image_in3d(
      openCLManager.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
      region[0], region[1], region[2], 0, 0, imageToProcess->data);
  cl::Image3D image_out3d(openCLManager.context, CL_MEM_WRITE_ONLY, format,
                          region[0], region[1], region[2]);
  kernel.setArg(0, image_in3d);
  kernel.setArg(1, image_out3d);
  performOperation(image_out3d);
  ellipseIn3d.reset();
  updateFullImage();
}

void ProcessingImage3d::getROIOOutOfMat() {
  if (!processROI) {
    imageToProcess = std::unique_ptr<Mat>(&image);
    return;
  }
  Rect imageRect(roi.first.first, roi.second.first,
                 roi.first.second - roi.first.first,
                 roi.second.second - roi.second.first);

  roiImage3d =
      Image3d(image3d.getDepth(), Mat(image3d.getImageAtDepth(0), imageRect));
  for (int i = 0; i < image3d.getDepth(); ++i) {
    roiImage3d.setImageAtDepth(
        i, Mat(image3d.getImageAtDepth(i), imageRect).clone());
  }
  roiImage = roiImage3d.get3dMatImage().clone();
  imageToProcess.reset(&roiImage);
  region[0] = roiImage3d.getCols();
  region[1] = roiImage3d.getRows();
  region[2] = roiImage3d.getDepth();
}

void ProcessingImage3d::updateFullImage() {
  if (!processROI) {
    image = *imageToProcess;
    return;
  }
  roiImage3d.set3dImage(*imageToProcess);
  Rect imageRect(roi.first.first, roi.second.first, roiImage3d.getCols(),
                 roiImage3d.getRows());
  std::cout << "Starting inserting images" << std::endl;
  for (int i = 0; i < image3d.getDepth(); ++i) {
    cv::Mat fullImageDepth(image3d.getRows(), image3d.getCols(),
                           imageToProcess->type(), 0.0);
    roiImage3d.getImageAtDepth(i).copyTo(fullImageDepth(imageRect));
    image3d.setImageAtDepth(i, fullImageDepth);
  }
  std::cout << "Images set" << std::endl;
  image = image3d.get3dMatImage();
}
}
