#pragma once

#include "ApplicationManager.h"

#include "GUI/cvImageWindow.h"
namespace Mgr {
class ApplicationManagerGUI : public QObject, public ApplicationManager {
  Q_OBJECT
private:
  std::unique_ptr<cvImageWindow> processedWindow;
  std::unique_ptr<cvImageWindow> originalWindow;
  std::unique_ptr<cvImageWindow> colsProcessed;
  std::unique_ptr<cvImageWindow> colsOriginal;
  std::unique_ptr<cvImageWindow> rowsProcessed;
  std::unique_ptr<cvImageWindow> rowsOriginal;
  void showWindows(const int &depth);
  void showCols(const int &col);
  void setMaxValues();
  void showRows(const int &row);

public:
  ApplicationManagerGUI(std::shared_ptr<OpenCLManager> openCLManager)
    : ApplicationManager(openCLManager) {}
  void showImages();
  void closeWindows();

public slots:
  void sliderValueChanged(const int &value, const QString &title);
  void setRectangle(QPoint startPoint, QPoint endPoint);
};
}
