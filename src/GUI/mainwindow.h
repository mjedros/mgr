#pragma once

#include "GUI/ApplicationManagerGUI.h"
#include "ContinuousProcessingMananger.h"
#include "OpenCLManager.h"
#include "GUI/cvImageWindow.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QStringListModel>
#include <thread>

namespace Ui {
class MainWindow;
}
namespace Mgr {
class ApplicationManagerGUI;
class Image3d;
class OpenCLManager;
enum SourceType : u_int8_t;
}
class VTKView;
class QStringListModel;
class MainWindow : public QMainWindow {
  Q_OBJECT
  typedef std::pair<std::pair<u_int16_t, u_int16_t>,
                    std::pair<u_int16_t, u_int16_t>> ROI;

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  void initBinaryImage();

  void process();
private slots:
  void on_ChoosePlatform_currentIndexChanged(const QString &description);
  void on_Process_clicked();
  void on_LoadImages_clicked();
  void on_ShowWindows_clicked();
  void on_CloseWindows_clicked();
  void openFileToProcess();
  void openDirToProcess();
  void on_Normalize_clicked();
  void on_ResetProcessed_clicked();
  void on_SaveImage_clicked();
  void on_Revert_clicked();

  void on_CameraAquisition_clicked();
  void on_UpdateOperation_clicked();

  void on_vtkViewButton_clicked();

  void on_saveCsvFile_clicked();
  void on_addToCsvFile_clicked();
  void on_loadCsvFile_clicked();
  void on_processCsvSequence_clicked();
  void on_deleteFromCsvFile_clicked();

private:
  std::unique_ptr<std::thread> processingThread;
  std::thread aquisitionThread;
  std::thread vtkViewThread;

  Mgr::cvImageWindow originalImage{ "", this };
  Mgr::cvImageWindow processedImage{ "after", this };

  ROI roi;
  std::pair<int, int> chosenDevice;
  std::vector<std::tuple<int, int, std::string>> listPlatforms;
  std::unique_ptr<Ui::MainWindow> ui;
  QStringListModel csvOperationsModel;
  QMenu menu;
  QMenuBar menu_bar;
  QString filename;
  QString directory;
  VTKView vtkView;
  Mgr::OpenCLManager openCLManager;
  Mgr::ApplicationManagerGUI applicationManager;
  Mgr::ContinuousProcessingMananger cameraProc;
  virtual void closeEvent(QCloseEvent *event);
  void setPlatformsList(void);
  void initImages(const Mgr::SourceType &source, const std::string &name);
  void updateCSVOperations();
  void startAquisition();
  std::tuple<std::string, std::string, const std::vector<float>>
  getProcessingValues();
public slots:
  void drawObject(cv::Mat);
  void drawProcessed(cv::Mat);
  void drawVtkImage(const std::shared_ptr<Mgr::Image3d> &);
  void showVtkImage() {
    vtkView.render();
    vtkView.show();
  }
};
