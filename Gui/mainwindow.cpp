#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "../src/OpenCLManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);
  setPlatformsList();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_ChoosePlatform_currentIndexChanged(const QString &arg1) {
  std::cout << arg1.toUtf8().constData() << std::endl;
}

void MainWindow::setPlatformsList() {
  OpenCLManager openCLManager;
  openCLManager.ChooseDevice(0, 2);
  std::vector<std::string> ListPlatforms = openCLManager.ListPlatforms();
  for (unsigned int i = 0; i < ListPlatforms.size(); ++i) {
    ui->ChoosePlatform->addItem(QString::fromStdString(ListPlatforms[i]));
  }
  openCLManager.Configure("./HelloWorld.cl", 0, 0);
}
