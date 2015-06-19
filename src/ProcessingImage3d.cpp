#include "ProcessingImage3d.h"

#include "Image3d.h"
#include "Logger.h"
#include "OpenCLManager.h"

using namespace cl;
using namespace cv;

namespace Mgr {
static Logger &logger = Logger::getInstance();
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
  if (strElementMap[structuralElementType.c_str()] ==
      StructuralElement::ELLIPSE) {
    const cl_float3 ellipseParams = { { structuralElementParams[0],
                                        structuralElementParams[1],
                                        structuralElementParams[2] } };
    kernel.setArg(2, ellipseParams);
  } else {
    const cl_int3 params = { {(int)structuralElementParams[0],
                              (int)structuralElementParams[1],
                              (int)structuralElementParams[2] } };
    kernel.setArg(2, params);
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
  cl::Event event;
  openCLManager.queue.enqueueNDRangeKernel(
      kernel, cl::NDRange(0, 0), cl::NDRange(region[0], region[1], region[2]),
      cl::NullRange, NULL, &event);
  event.wait();
  openCLManager.queue.enqueueReadImage(image_out3d, CL_TRUE, origin, region, 0,
                                       0, imageToProcess->data);
  const auto elapsed = event.getProfilingInfo<CL_PROFILING_COMMAND_END>() -
                       event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  logger.endOperation(elapsed);
  updateFullImage();
}
}
