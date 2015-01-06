#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "Image3d.h"
#include "GUI/cvImageWindow.h"
#include "ImageSource/SourceFactory.h"
#include <QObject>
#include <QEvent>
namespace Mgr {
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };
class ApplicationManager {
  protected:
    std::shared_ptr<OpenCLManager> openCLManager;
    std::string sourceFilename;
    std::shared_ptr<Image3d> image3d;
    std::shared_ptr<Image3d> processedImage3d;

  public:
    ApplicationManager(std::shared_ptr<OpenCLManager> openCLManagerPtr)
        : openCLManager(std::move(openCLManagerPtr)) {}
    void process(const OPERATION &operation,
                 const std::string &structuralElement);
    void init(const SourceType &source, const std::string &name);
    void initProcessedImage(const unsigned int &minumum = 100,
                            const unsigned int &maximum = 255);
    void normalizeOriginalImage();
};
}
#endif // APPLICATIONMANAGER_H
