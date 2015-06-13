#include "ProcessingImage3d.h"

#include "Image3d.h"
#include "OpenCLManager.h"

using namespace cl;
using namespace cv;

namespace Mgr {

ProcessingImage3d::ProcessingImage3d(
    const std::shared_ptr<OpenCLManager> &openCLManagerPtr, bool processRoi)
  : ProcessingImage(openCLManagerPtr, processRoi) {}

void ProcessingImage3d::setKernel(const std::string &Operation) {
  setKernelOperation(Operation + "3d");
}
void ProcessingImage3d::set3dImageToProcess(const Image3d &image3d) {
  image.release();
  image = image3d.get3dMatImage().clone();
  region[0] = image3d.getCols();
  region[1] = image3d.getRows();
  region[2] = image3d.getDepth();
  imageToProcess.release();
  imageToProcess = std::unique_ptr<Mat>(&image);
  const ImageFormat format(CL_R, CL_UNORM_INT8);

  cv::Mat imageOut(imageToProcess->rows, imageToProcess->cols,
                   imageToProcess->type());

  cl::Kernel kernel = cl::Kernel(openCLManager->program, "image3dTest");
  cl::Image3D image_in3d(
      openCLManager->context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
      region[0], region[1], region[2], 0, 0, imageToProcess->data);
  cl::Image3D image_out3d(openCLManager->context, CL_MEM_WRITE_ONLY, format,
                          region[0], region[1], region[2]);
  kernel.setArg(0, image_in3d);
  kernel.setArg(1, image_out3d);

  openCLManager->queue.enqueueNDRangeKernel(
      kernel, cl::NDRange(0, 0), cl::NDRange(region[0], region[1], region[2]),
      cl::NullRange, NULL);
  openCLManager->queue.enqueueReadImage(image_out3d, CL_TRUE, origin, region, 0,
                                        0, imageOut.data);
}
}
