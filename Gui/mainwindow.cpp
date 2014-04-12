#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "../src/ApplicationManager.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
   openCLManager = std::make_shared<OpenCLManager>();
   ui->setupUi(this);
   setPlatformsList();
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::on_ChoosePlatform_currentIndexChanged(const QString &description)
{
   auto it = std::find_if(ListPlatforms.begin(), ListPlatforms.end(),
                          [&](std::tuple<int, int, std::string> &platform)
                          { return (description.toStdString() == std::get<2>(platform)); });
   ChosenDevice = std::make_pair(std::get<0>(*it), std::get<1>(*it));
}

void MainWindow::setPlatformsList()
{
   ListPlatforms = openCLManager->ListPlatforms();
   std::for_each(ListPlatforms.begin(), ListPlatforms.end(),
                 [this](std::tuple<int, int, std::string> &platform)
                 { ui->ChoosePlatform->addItem(QString::fromStdString(std::get<2>(platform))); });
}

void MainWindow::on_pushButton_clicked()
{

   openCLManager->Configure("../Kernels/Kernels.cl", ChosenDevice.first, ChosenDevice.second);
   ApplicationManager appManager(openCLManager);
   ui->pushButton->hide();
   appManager.DoSth();
}
