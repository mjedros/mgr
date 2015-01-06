#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include "../src/OpenCLManager.h"
namespace Ui {
class MainWindow;
}
namespace Mgr {
class ApplicationManagerGUI;
class OpenCLManager;
enum SourceType : u_int8_t;
}
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
  private slots:
    void on_ChoosePlatform_currentIndexChanged(const QString &description);
    void on_Process_clicked();
    void on_LoadImages_clicked();
    void on_ShowWindows_clicked();
    void openFileToProcess();
    void openDirToProcess();
    void on_Normalize_clicked();
    void on_ResetProcessed_clicked();
    void on_SaveImage_clicked();

  private:
    std::pair<int, int> chosenDevice;
    std::vector<std::tuple<int, int, std::string> > listPlatforms;
    Ui::MainWindow *ui;
    QMenu *menu;
    QMenuBar menu_bar;
    QString filename;
    QString directory;
    std::shared_ptr<Mgr::OpenCLManager> openCLManager;
    std::unique_ptr<Mgr::ApplicationManagerGUI> applicationManager;
    virtual void closeEvent(QCloseEvent *event);
    void setPlatformsList(void);
    void initImages(const Mgr::SourceType &source, const std::string &name);
};

#endif // MAINWINDOW_H
