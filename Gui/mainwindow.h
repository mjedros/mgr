#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include "../src/OpenCLManager.h"
namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

 public:
   explicit MainWindow(QWidget *parent = 0);
   ~MainWindow();

 private slots:

   void on_ChoosePlatform_currentIndexChanged(const QString &description);
   void setPlatformsList(void);
   void on_Process_clicked();
   void openFileToProcess();

 private:
   std::pair<int, int> ChosenDevice;
   std::vector<std::tuple<int, int, std::string>> ListPlatforms;
   Ui::MainWindow *ui;
   std::shared_ptr<OpenCLManager> openCLManager;
   QMenu *menu;
   QMenuBar menu_bar;
   QString filename;
};

#endif // MAINWINDOW_H
