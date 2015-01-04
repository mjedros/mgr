#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "SourceFactory.h"
#include "Image3d.h"
#include "GUI/cvImageWindow.h"
#include <QObject>
#include <QWheelEvent>
#include <QEvent>

enum class OBJECT : uint8_t { MOVIE, DIRECTORY };
enum class OPERATION : uint8_t { DILATION, EROSION, CONTOUR, SKELETONIZATION };
class ApplicationManager : public QObject {
    Q_OBJECT
  private:
    std::shared_ptr<OpenCLManager> openCLManager;
    std::string sourceFilename;
    std::unique_ptr<Image3d> image3d;
    std::unique_ptr<Image3d> processedImage3d;
    std::unique_ptr<cvImageWindow> processedWindow;
    std::unique_ptr<cvImageWindow> originalWindow;
    std::unique_ptr<cvImageWindow> colsProcessed;
    std::unique_ptr<cvImageWindow> colsOriginal;
    void showWindows(const int &depth);
    void showCols(const int &cols);
    void setMaxValues();
    void closeEvent(QCloseEvent *event);
    void saveMovie(const Image3d &img3d, const std::string &filename);

  public:
    ApplicationManager(std::shared_ptr<OpenCLManager> openCLManager);
    void loadDir3dImage(const std::string &Directory);
    void loadFile3dImage(const std::string &filename);
    void initWindows(const OBJECT &object, const std::string &name);
    void showImages();
    void process(const OPERATION &operation,
                 const std::string &StructuralElement);
  public slots:
    void sliderValueChanged(const int &value, const QString &title);
};

#endif // APPLICATIONMANAGER_H
