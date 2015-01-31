#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "Image3d.h"
#include "GUI/cvImageWindow.h"
#include <QObject>
#include <QEvent>
namespace Mgr {
enum SourceType : u_int8_t;
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };
class ApplicationManager {
  protected:
    std::shared_ptr<OpenCLManager> openCLManager;
    std::string sourceFilename;
    std::shared_ptr<Image3d> image3d;
    std::shared_ptr<Image3d> processedImage3d;

  public:
    ApplicationManager(const std::shared_ptr<OpenCLManager> &openCLManagerPtr)
        : openCLManager(openCLManagerPtr) {}
    void process(const OPERATION &operation,
                 const std::string &structuralElement,
                 const std::vector<float> &params);
    void init(const SourceType &source, const std::string &name);
    void initProcessedImage(const unsigned int &minumum = 100,
                            const unsigned int &maximum = 255);
    void normalizeOriginalImage();
    void saveOriginalImage(const std::string &filename);
    void saveProcessedImage(const std::string &filename);
};
}
#endif // APPLICATIONMANAGER_H
