#pragma once

#include "OpenCLManager.h"

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <opencv2/opencv.hpp>
#include <memory>

namespace Mgr {

enum StructuralElement : u_int8_t { ELLIPSE, CROSS, RECTANGLE, ELLIPSEIMG };
typedef std::pair<std::pair<u_int16_t, u_int16_t>,
                  std::pair<u_int16_t, u_int16_t>> ROI;

class OpenCLManager;
enum class OPERATION : uint8_t {
  DILATION,
  DILATIONIMG,
  EROSION,
  OPENING,
  CLOSING,
  CONTOUR,
  SKELETONIZATION,
  SKELETONIZATION2
};
using OperationsVector = std::vector<std::vector<std::string>>;
/**
 * @brief Class representing image that is being processed with OpenCL
 */
class Image3d;
class ProcessingImage {
  std::unique_ptr<cl::Image2D> image_in_ptr = nullptr;
  std::unique_ptr<cl::Image2D> image_out_ptr = nullptr;

protected:
  bool processingSequence = false;
  cv::Mat image;
  cv::Mat roiImage;
  cv::Mat *imageToProcess = nullptr;
  cl::size_t<3> origin;
  cl::size_t<3> region;
  cl::NDRange localRange;
  cl::Kernel kernel;
  ROI roi;
  bool newImage = true;
  OpenCLManager &openCLManager;
  std::string structuralElementType;
  std::string kernelOperation;
  std::vector<float> structuralElementParams;
  bool processROI = false;
  bool processOpenCV = false;
  void process(cl::Kernel &kernel, cl::Image2D &image_in,
               cl::Image2D &image_out);
  void setNew(bool newim) { newImage = newim; }
  virtual void getROIOOutOfMat();
  virtual void updateFullImage();

  std::map<std::string, StructuralElement> strElementMap;

public:
  static std::map<std::string, OPERATION> OperationMap;
  /**
   * @brief Performs binary operation with mininum and maximum value
   * @param minimum
   * @param maximum
   */
  void binarize(const unsigned int &minumum = 127,
                const unsigned int &maximum = 255);
  /**
   * @brief Performs dilation
   */
  void dilate();
  /**
   * @brief Performs erosion
   */
  void erode();
  /**
   * @brief Derives contour of binary image
   */
  void contour();

  void open();
  void close();
  /**
   * @brief Performs skeletonization
   */
  void skeletonize();

  virtual void skeletonize2();
  /**
   * @brief Performs specific morphological operation
   * @param Operation  name of operation in kernel file
   */
  virtual void performMorphologicalOperation();
  /**
   * @brief Set structural element for morphological operations
   * @param element Type of element
   * @param params sizes of element
   */

  void setStructuralElement(const std::string &element,
                            const std::vector<float> &params);
  /**
   * @brief Image getter
   * @return Image data
   */
  cv::Mat getImage() const;
  /**
   * @brief Set image data
   * @param img - image sent to process
   */
  virtual void substr();
  virtual void readImage();
  void processSequence(OperationsVector vect);
  void setImageToProcess(const cv::Mat &img);
  void setROI(const ROI NewRoi) { roi = NewRoi; }
  void setProcessCV(const bool processCv) { processOpenCV = processCv; }
  ProcessingImage(OpenCLManager &openCLManagerPtr, bool processRoi = false);
  ~ProcessingImage() {}
  inline void setKernelOperation(const std::string &Operation) {
    kernelOperation = Operation;
    const std::string kernelName = Operation + structuralElementType;
    kernel = cl::Kernel(openCLManager.program, kernelName.c_str());
    setStructuralElementArgument();
  }
  virtual void setKernel(const std::string &Operation);
  void setKernelWithOperation(const OPERATION &Operation);

  virtual void setStructuralElementArgument();
};
}
