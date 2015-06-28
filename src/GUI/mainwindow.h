#pragma once

#include "GUI/ApplicationManagerGUI.h"
#include "OpenCLManager.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QStringListModel>
#include <thread>

namespace Ui {
class MainWindow;
}
namespace Mgr {
class ApplicationManagerGUI;
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

  void process(const std::vector<float> StructElemParams,
               const std::string MorphElementType);
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

  void on_vtkViewButton_clicked();
  void on_addNextVTKImage_clicked();

  void on_saveCsvFile_clicked();
  void on_addToCsvFile_clicked();
  void on_loadCsvFile_clicked();
  void on_processCsvSequence_clicked();
  void on_deleteFromCsvFile_clicked();

private:
  std::unique_ptr<std::thread> processingThread;
  std::unique_ptr<std::thread> enqueImagesThread;
  ROI roi;
  std::pair<int, int> chosenDevice;
  std::vector<std::tuple<int, int, std::string>> listPlatforms;
  std::unique_ptr<Ui::MainWindow> ui;
  QStringListModel csvOperationsModel;
  QMenu menu;
  QMenuBar menu_bar;
  QString filename;
  QString directory;
  std::unique_ptr<VTKView> vtkView;
  Mgr::OpenCLManager openCLManager;
  Mgr::ApplicationManagerGUI applicationManager;
  virtual void closeEvent(QCloseEvent *event);
  void setPlatformsList(void);
  void initImages(const Mgr::SourceType &source, const std::string &name);
  void updateCSVOperations();
};
