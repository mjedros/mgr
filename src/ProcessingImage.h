#ifndef PROCESSINGIMAGE_H
#define PROCESSINGIMAGE_H
#include <opencv2/opencv.hpp>
#include "OpenCLManager.h"

namespace Mgr {
int getAvarage();
void clear();
enum StructuralElement : u_int8_t { ELLIPSE, CROSS, RECTANGLE };

/**
 * @brief Class representing image that is being processed with OpenCL
 */
class ProcessingImage {
private:
  cv::Mat image;
  cl::size_t<3> origin;
  cl::size_t<3> region;
  cl::NDRange localRange;
  void process(cl::Kernel &kernel, cl::Image2D &image_in,
               cl::Image2D &image_out);
  std::shared_ptr<OpenCLManager> openCLManager;
  std::string structuralElementType;
  std::vector<float> structuralElementParams;
  void setStructuralElementArgument(cl::Kernel &kernel);
  /**
   * @brief Performs specific morphological operation
   * @param Operation - name of operation in kernel file
   */
  void performMorphologicalOperation(const std::string &Operation);

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
  ProcessingImage(const std::shared_ptr<OpenCLManager> &openCLManagerPtr);
};
}
#endif // PROCESSINGIMAGE_H
