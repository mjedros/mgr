#pragma once

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <opencv2/opencv.hpp>
#include <memory>

namespace Mgr {

enum StructuralElement : u_int8_t { ELLIPSE, CROSS, RECTANGLE };
typedef std::pair<std::pair<u_int16_t, u_int16_t>,
                  std::pair<u_int16_t, u_int16_t>> ROI;

class OpenCLManager;
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };

/**
 * @brief Class representing image that is being processed with OpenCL
 */
class Image3d;
class ProcessingImage {
protected:
  cv::Mat image;
  cv::Mat roiImage;
  std::unique_ptr<cv::Mat> imageToProcess;
  cl::size_t<3> origin;
  cl::size_t<3> region;
  cl::NDRange localRange;
  cl::Kernel kernel;
  ROI roi;
  std::shared_ptr<OpenCLManager> openCLManager;
  std::string structuralElementType;
  std::vector<float> structuralElementParams;
  bool processROI;

  void process(cl::Kernel &kernel, cl::Image2D &image_in,
               cl::Image2D &image_out);

  void getROIOOutOfMat();
  void updateFullImage();

public:
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
  /**
   * @brief Performs skeletonization
   */
  void skeletonize();
  /**
   * @brief Performs specific morphological operation
   * @param Operation - name of operation in kernel file
   */
  void performMorphologicalOperation();
  /**
   * @brief Set structural element for morphological operations
   * @param element - Type of element
   * @param params - sizes of element
   */

  void setStructuralElement(const std::string &element,
                            const std::vector<float> &params);
  /**
   * @brief Image getter
   * @return Image data
   */
  cv::Mat getImage();
  /**
   * @brief Set image data
   * @param img - image sent to process
   */
  void setImageToProcess(const cv::Mat &img);
  void setROI(const ROI NewRoi) { roi = NewRoi; }
  ProcessingImage(const std::shared_ptr<OpenCLManager> &openCLManagerPtr,
                  bool processRoi = false);
  ~ProcessingImage() { imageToProcess.release(); }
  void setKernelOperation(const std::string &Operation);
  virtual void setKernel(const std::string &Operation);
  virtual void setKernelWithOperation(const OPERATION &Operation);
  void setStructuralElementArgument();
};
}
