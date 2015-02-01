#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include <QObject>
#include <QEvent>
namespace Mgr {
class Image3d;
enum SourceType : u_int8_t;
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };

class Processing3dImage {
  public:
    virtual void process(std::shared_ptr<Image3d> image,
                         const std::shared_ptr<ProcessingImage> img,
                         const OPERATION &operation) = 0;
};
class ProcessCols : public Processing3dImage {
  public:
    void process(std::shared_ptr<Image3d> image,
                 std::shared_ptr<ProcessingImage> img,
                 const OPERATION &operation);
};
class ProcessDepth : public Processing3dImage {
  public:
    void process(std::shared_ptr<Image3d> image,
                 const std::shared_ptr<ProcessingImage> img,
                 const OPERATION &operation);
};
class ApplicationManager {
  protected:
    std::shared_ptr<OpenCLManager> openCLManager;
    std::string sourceFilename;
    std::shared_ptr<Image3d> image3d;
    std::shared_ptr<Image3d> processedImage3d;

  public:
    ApplicationManager(const std::shared_ptr<OpenCLManager> &openCLManagerPtr)
        : openCLManager(openCLManagerPtr) {}
    template <class T>
    void process(const OPERATION &operation,
                 const std::string &structuralElement,
                 const std::vector<float> &params) {
        cv::waitKey(1);
        std::shared_ptr<ProcessingImage> img(
            new ProcessingImage(openCLManager));
        img->setStructuralElement(structuralElement, params);
        std::unique_ptr<Processing3dImage> processing3dImage;
        processing3dImage = std::unique_ptr<T>(new T);
        processing3dImage->process(processedImage3d, img, operation);
    }

    void init(const SourceType &source, const std::string &name);
    void initProcessedImage(const unsigned int &minumum = 100,
                            const unsigned int &maximum = 255);
    void normalizeOriginalImage();
    void saveOriginalImage(const std::string &filename);
    void saveProcessedImage(const std::string &filename);
};
}
#endif // APPLICATIONMANAGER_H
