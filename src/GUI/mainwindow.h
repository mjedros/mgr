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

  private:
    virtual void closeEvent(QCloseEvent *event);
    std::pair<int, int> chosenDevice;
    std::vector<std::tuple<int, int, std::string> > listPlatforms;
    Ui::MainWindow *ui;
    QMenu *menu;
    QMenuBar menu_bar;
    QString filename;
    QString directory;
    void setPlatformsList(void);
    std::shared_ptr<Mgr::OpenCLManager> openCLManager;
    std::unique_ptr<Mgr::ApplicationManagerGUI> applicationManager;
};

#endif // MAINWINDOW_H
