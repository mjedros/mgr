#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "../src/OpenCLManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{

   ui->setupUi(this);
   setPlatformsList();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_ChoosePlatform_currentIndexChanged(const QString &arg1)
{
   auto it = std::find_if(ListPlatforms.begin(), ListPlatforms.end(),
                          [&](std::tuple<int, int, std::string> &platform)
   { return (arg1.toStdString() == std::get<2>(platform)); });
   ChosenDevice = std::make_pair(std::get<0>(*it), std::get<1>(*it));
}

void MainWindow::setPlatformsList()
{
   OpenCLManager openCLManager;
   ListPlatforms = openCLManager.ListPlatforms();
   std::for_each(ListPlatforms.begin(), ListPlatforms.end(),
                 [this](std::tuple<int, int, std::string> &platform)
   {
      ui->ChoosePlatform->addItem(
          QString::fromStdString(std::get<2>(platform)));
   });
}
