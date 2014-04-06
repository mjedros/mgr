#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
   void on_pushButton_clicked();

private:
   std::pair<int, int> ChosenDevice;
   std::vector<std::tuple<int, int, std::string> > ListPlatforms;
   Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
