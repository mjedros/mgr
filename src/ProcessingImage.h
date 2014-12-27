#ifndef PROCESSINGIMAGE_H
#define PROCESSINGIMAGE_H
#include <opencv2/opencv.hpp>
#include "OpenCLManager.h"

namespace CLProcessingImage {
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
    void Process(cl::Kernel &kernel, const cl::Image2D &image_in,
                 cl::Image2D &image_out);
    std::shared_ptr<OpenCLManager> openCLManager;
    std::string structuralElementType;
    std::vector<float> structuralElementParams;
    void SetStructuralElementArgument(cl::Kernel &kernel);
    /**
     * @brief Performs specific morphological operation
     * @param Operation - name of operation in kernel file
     */
    void PerformMorphologicalOperation(const std::string &Operation);

  public:
    /**
     * @brief Performs binary operation with mininum and maximum value
     * @param minimum
     * @param maximum
     */
    void Binarize(const unsigned int &minumum = 127,
                  const unsigned int &maximum = 255);
    /**
     * @brief Performs dilation
     */
    void Dilate();
    /**
     * @brief Performs erosion
     */
    void Erode();
    /**
     * @brief Derives contour of binary image
     */
    void Contour();
    /**
     * @brief Performs skeletonization
     */
    void Skeletonize();
    /**
     * @brief Set structural element for morphological operations
     * @param element - Type of element
     * @param params - sizes of element
     */
    void SetStructuralElement(const std::string &element,
                              const std::vector<float> &params);
    /**
     * @brief Image getter
     * @return Image data
     */
    cv::Mat GetImage();
    /**
     * @brief Set image data
     * @param img - image sent to process
     */
    void SetImageToProcess(cv::Mat img);
    ProcessingImage(std::shared_ptr<OpenCLManager> openCLManagerPtr);
};
}
#endif // PROCESSINGIMAGE_H
