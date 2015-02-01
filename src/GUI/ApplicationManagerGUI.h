#ifndef APPLICATIONMANAGERGUI_H
#define APPLICATIONMANAGERGUI_H
#include "../src/ApplicationManager.h"
#include "cvImageWindow.h"
namespace Mgr {
class ApplicationManagerGUI : public QObject, public ApplicationManager {
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
    ApplicationManagerGUI(std::shared_ptr<OpenCLManager> openCLManager)
        : ApplicationManager(openCLManager) {}
    void showImages();

  public slots:
    void sliderValueChanged(const int &value, const QString &title);
};
}
#endif // APPLICATIONMANAGERGUI_H
