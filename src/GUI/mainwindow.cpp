#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "../src/ApplicationManager.h"
#include <QFileDialog>
#include "../src/include/Paths.h"
using namespace Mgr;
std::map<QString, OPERATION> OperationMap = {
    { "Dilation", OPERATION::DILATION },
    { "Erosion", OPERATION::EROSION },
    { "Contour", OPERATION::CONTOUR },
    { "Skeletonize", OPERATION::SKELETONIZATION }
};
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), menu(new QMenu("File")),
      openCLManager(new OpenCLManager()),
      appManager(new ApplicationManager(openCLManager)) {
    this->setFixedSize(this->size());
    ui->setupUi(this);
    setPlatformsList();
    menu->addAction("Open file to process", this, SLOT(openFileToProcess()));
    menu->addAction("Open directory to process", this,
                    SLOT(openDirToProcess()));
    menu_bar.addMenu(menu);
    ui->ChooseOperation->addItems(
        { "Dilation", "Erosion", "Contour", "Skeletonize" });
    ui->MorphologicalElementType->addItems({ "Ellipse", "Cross", "Rectangle" });
    setMenuBar(&menu_bar);
}

MainWindow::~MainWindow() {
    delete ui;
    delete menu;
}

void
MainWindow::on_ChoosePlatform_currentIndexChanged(const QString &description) {
    auto it = std::find_if(listPlatforms.begin(), listPlatforms.end(),
                           [&](std::tuple<int, int, std::string> &platform) {
        return (description.toStdString() == std::get<2>(platform));
    });
    chosenDevice = std::make_pair(std::get<0>(*it), std::get<1>(*it));
}

void MainWindow::setPlatformsList() {
    listPlatforms = openCLManager->listPlatforms();
    std::for_each(listPlatforms.begin(), listPlatforms.end(),
                  [this](std::tuple<int, int, std::string> &platform) {
        ui->ChoosePlatform->addItem(
            QString::fromStdString(std::get<2>(platform)));
    });
}

void MainWindow::on_Process_clicked() {
    ui->ProcessingProgress->setEnabled(true);
    ui->ProcessingProgress->setText("In progress");
    ui->Process->hide();
    appManager->process(
        OperationMap[ui->ChooseOperation->currentText()],
        ui->MorphologicalElementType->currentText().toStdString());
    ui->Process->show();
    ui->ProcessingProgress->setText("Done");
}

void MainWindow::openFileToProcess() {
    filename = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                            QDir::currentPath());
}
void MainWindow::openDirToProcess() {
    directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  QDir::currentPath());
}

void MainWindow::on_LoadImages_clicked() {
    openCLManager->configure(std::string(KERNELS_DIR) + "Kernels.cl",
                             chosenDevice);
    if (ui->File->isChecked()) {
        if (filename.size() != 0) {
            appManager->init(OBJECT::MOVIE, filename.toStdString());
            ui->ImagesLoaded->setText("Images Loaded");
        }
    } else {
        if (directory.size() != 0) {
            appManager->init(OBJECT::DIRECTORY,
                                    directory.toStdString() + "/");
            ui->ImagesLoaded->setText("Images Loaded");
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    appManager.reset();
    QMainWindow::closeEvent(event);
}

void MainWindow::on_ShowWindows_clicked() { appManager->showImages(); }
