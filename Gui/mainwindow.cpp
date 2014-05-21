#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "../src/ApplicationManager.h"
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
   openCLManager = std::make_shared<OpenCLManager>();
   ui->setupUi(this);
   setPlatformsList();
   menu = new QMenu("File");
   menu->addAction("Open file to process", this, SLOT(openFileToProcess()));
   menu_bar.addMenu(menu);

   setMenuBar(&menu_bar);
}

MainWindow::~MainWindow()
{
   delete ui;
   openCLManager.reset();
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

void MainWindow::on_Process_clicked()
{
   openCLManager->Configure("../Kernels/Kernels.cl", ChosenDevice);
   ApplicationManager appManager(openCLManager);
   ui->Process->hide();
   appManager.SetFileToProcess(filename.toStdString());
   appManager.DoSth();
   ui->Process->show();
}

void MainWindow::openFileToProcess()
{
   filename = QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::currentPath());
}
