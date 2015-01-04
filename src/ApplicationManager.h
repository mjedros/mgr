#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "Image3d.h"
#include "GUI/cvImageWindow.h"
#include <QObject>
#include <QEvent>
namespace Mgr {
enum class OBJECT : uint8_t { MOVIE, DIRECTORY };
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };
class AppMan {
  protected:
    std::shared_ptr<OpenCLManager> openCLManager;
    std::string sourceFilename;
    std::unique_ptr<Image3d> image3d;
    std::shared_ptr<Image3d> processedImage3d;

  public:
    AppMan(std::shared_ptr<OpenCLManager> openCLManagerPtr)
        : openCLManager(std::move(openCLManagerPtr)) {}
    void loadDir3dImage(const std::string &Directory);
    void loadFile3dImage(const std::string &filename);
    void process(const OPERATION &operation,
                 const std::string &structuralElement);
    void init(const OBJECT &object, const std::string &name);
};

class ApplicationManager : public QObject, public AppMan {
    Q_OBJECT
  private:
    std::unique_ptr<cvImageWindow> processedWindow;
    std::unique_ptr<cvImageWindow> originalWindow;
    std::unique_ptr<cvImageWindow> colsProcessed;
    std::unique_ptr<cvImageWindow> colsOriginal;
    void showWindows(const int &depth);
    void showCols(const int &cols);
    void setMaxValues();
    void closeEvent(QCloseEvent *event);

  public:
    ApplicationManager(std::shared_ptr<OpenCLManager> openCLManager) : AppMan(openCLManager){}
    void showImages();

  public slots:
    void sliderValueChanged(const int &value, const QString &title);
};
}
#endif // APPLICATIONMANAGER_H
