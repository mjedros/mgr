#include "ProcessingImage3d.h"

#include "Image3d.h"
#include "Logger.h"
#include "OpenCLManager.h"

using namespace cl;
using namespace cv;

namespace Mgr {
static Logger &logger = Logger::getInstance();
namespace {
cv::Mat getEllipse(int xRadius, int y, int rows, int cols, int type) {
  cv::Mat ellipse(rows, cols, type, cv::Scalar(0, 0, 0));

  cv::Point point(rows / 2, cols / 2);
  std::cout << "x,y :" << xRadius << ", " << y << std::endl;
  cv::Size size(y, xRadius);

  cv::ellipse(ellipse, point, size, 0, 0, 360, cv::Scalar(255, 255, 255), -1,
              0);
  return ellipse;
}
cv::Mat getEllipsoidImage(std::vector<float> params, cv::Mat img) {
  Image3d ellipse3d(params[2] * 2 + 1,
                    cv::Mat(params[0] * 2 + 1, params[1] * 2 + 1, img.type()));
  const auto cols = ellipse3d.getCols();
  const auto rows = ellipse3d.getRows();
  const float radius = params[2];
  for (float i = 0; i <= radius; i++) {

    float xRadius = (float)params[0] *
                    sqrt(1.0 - (radius - i) * (radius - i) / (radius * radius));
    float yRadius = (float)params[1] *
                    sqrt(1.0 - (radius - i) * (radius - i) / (radius * radius));
    std::cout << "Radiuses x,y :" << xRadius << ", " << yRadius << std::endl;

    if ((int)xRadius == 0) {
      cv::Mat xyEllipse(rows, cols, img.type());
      xyEllipse.at<uchar>(rows / 2, cols / 2) = 255;
      ellipse3d.setImageAtDepth(i, xyEllipse);
      ellipse3d.setImageAtDepth(ellipse3d.getDepth() - i - 1, xyEllipse);
      continue;
    }
    auto ellipse = getEllipse(xRadius, yRadius, ellipse3d.getRows(),
                              ellipse3d.getCols(), img.type());
    ellipse3d.setImageAtDepth(i, ellipse);
    ellipse3d.setImageAtDepth(ellipse3d.getDepth() - i - 1, ellipse);
  }
  cv::imwrite("~/ellipseDepth.jpg", ellipse3d.getImageAtDepth(params[2] / 2));
  return ellipse3d.get3dMatImage();
}
}

ProcessingImage3d::ProcessingImage3d(OpenCLManager &openCLManagerRef,
                                     bool processRoi)
  : ProcessingImage(openCLManagerRef, processRoi) {}

void ProcessingImage3d::set3dImageToProcess(const Image3d &image3d) {
  image.release();
  image = image3d.get3dMatImage().clone();
  region[0] = image3d.getCols();
  region[1] = image3d.getRows();
  region[2] = image3d.getDepth();
  imageToProcess.release();
  imageToProcess = std::unique_ptr<Mat>(&image);
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

    cv::imwrite("~/ellipse.jpg", ellipsoidImage);

    const cl_float3 ellipseParams = { { structuralElementParams[0],
                                        structuralElementParams[1],
                                        structuralElementParams[2] } };
    kernel.setArg(3, ellipseParams);
    break;
  }
  default:
    break;
  }
}

void ProcessingImage3d::performOperation(Image3D &image_out3d) {
  std::cout << "Performing morph operation" << std::endl;
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
    updateFullImage();
  } catch (cl::Error &e) {
    logger.printError(e);
  }
}

void ProcessingImage3d::performMorphologicalOperation() {
  const ImageFormat format(CL_R, CL_UNORM_INT8);

  logger.beginOperation();
  cl::Image3D image_in3d(
      openCLManager.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
      region[0], region[1], region[2], 0, 0, imageToProcess->data);
  cl::Image3D image_out3d(openCLManager.context, CL_MEM_WRITE_ONLY, format,
                          region[0], region[1], region[2]);
  kernel.setArg(0, image_in3d);
  kernel.setArg(1, image_out3d);
  if (strElementMap[structuralElementType.c_str()] == ELLIPSEIMG) {
    cl::Image3D ellipseIn3d(
        openCLManager.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
        structuralElementParams[1] * 2 + 1, structuralElementParams[0] * 2 + 1,
        structuralElementParams[2] * 2 + 1, 0, 0, ellipsoidImage.data);
    kernel.setArg(2, ellipseIn3d);

    performOperation(image_out3d);
  } else
    performOperation(image_out3d);
}
}
