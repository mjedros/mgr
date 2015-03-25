#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "ApplicationManagerGUI.h"
#include <QFileDialog>
#include "../build/src/include/Paths.h"
#include "../ImageSource/SourceFactory.h"
#include "vtkview.h"
#include "../src/Image3d.h"
using namespace Mgr;
std::map<QString, OPERATION> OperationMap = {
  { "Dilation", OPERATION::DILATION },
  { "Erosion", OPERATION::EROSION },
  { "Contour", OPERATION::CONTOUR },
  { "Skeletonize", OPERATION::SKELETONIZATION }
};
namespace {
inline std::string getDoubleText(const QString &text) {
  std::string doubleText = text.toStdString();
  doubleText.replace(doubleText.find(','), 1, ".");
  return doubleText;
}
}
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow), menu(new QMenu("File")),
    openCLManager(new OpenCLManager()),
    applicationManager(new ApplicationManagerGUI(openCLManager)) {
  ui->setupUi(this);
  setPlatformsList();
  menu->addAction("Open file to process", this, SLOT(openFileToProcess()));
  menu->addAction("Open directory to process", this, SLOT(openDirToProcess()));
  menu_bar.addMenu(menu);
  ui->ChooseOperation->addItems(
      { "Dilation", "Erosion", "Contour", "Skeletonize" });
  ui->MorphologicalElementType->addItems({ "Ellipse", "Cross", "Rectangle" });
  ui->ProcessingWay->addItems({ "Process columns", "Process depth" });
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
    ui->ChoosePlatform->addItem(QString::fromStdString(std::get<2>(platform)));
  });
}

void MainWindow::initImages(const Mgr::SourceType &source,
                            const std::string &name) {
  applicationManager->init(source, name);
  initBinaryImage();
  ui->ImagesLoaded->setText("Images Loaded");
  ui->Process->setEnabled(true);
  ui->ShowWindows->setEnabled(true);
  ui->ResetProcessed->setEnabled(true);
  ui->Normalize->setEnabled(true);
  ui->SaveImage->setEnabled(true);
}

void MainWindow::on_Process_clicked() {
  ui->ProcessingProgress->setEnabled(true);
  ui->ProcessingProgress->setText("In progress");
  cv::waitKey(1);

  const OPERATION &operation = OperationMap[ui->ChooseOperation->currentText()];
  const std::string MorphElementType =
      ui->MorphologicalElementType->currentText().toStdString();
  const std::vector<float> StructElemParams = {
    static_cast<float>(ui->StructElementParam1->value()),
    static_cast<float>(ui->StructElementParam2->value()),
    static_cast<float>(ui->StructElementParam3->value())
  };
  if (ui->ProcessingWay->currentText() == "Process columns")
    applicationManager->process<ProcessCols>(operation, MorphElementType,
                                             StructElemParams);
  else
    applicationManager->process<ProcessDepth>(operation, MorphElementType,
                                              StructElemParams);
  ui->Process->show();
  ui->ProcessingProgress->setText("Done");
}

void MainWindow::openFileToProcess() {
  filename =
      QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::currentPath());
}
void MainWindow::openDirToProcess() {
  directory = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                QDir::currentPath());
}

void MainWindow::on_LoadImages_clicked() {
  ui->ImagesLoaded->setText("Images not loaded!");
  openCLManager->configure(std::string(KERNELS_DIR) + "Kernels.cl",
                           chosenDevice);
  if (ui->File->isChecked() && filename.size() != 0)
    initImages(VideoFile, filename.toStdString());
  else if (ui->Directory->isChecked() && directory.size() != 0)
    initImages(DirectorySource, directory.toStdString() + "/");
}

void MainWindow::closeEvent(QCloseEvent *event) {
  applicationManager.reset();
  QMainWindow::closeEvent(event);
}

void MainWindow::on_ShowWindows_clicked() { applicationManager->showImages(); }

void MainWindow::on_Normalize_clicked() {
  applicationManager->normalizeOriginalImage();
  initBinaryImage();
}

void MainWindow::initBinaryImage() {
  applicationManager->initProcessedImage(ui->lowLewel->value(),
                                         ui->highLevel->value());
}

void MainWindow::on_ResetProcessed_clicked() { initBinaryImage(); }

void MainWindow::on_SaveImage_clicked() {
  applicationManager->saveOriginalImage(
      QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath())
          .toStdString());
}

void MainWindow::on_vtkViewButton_clicked() {
  vtkView.reset(new VTKView());
  vtkView->setImage3d(applicationManager->getProcessedImage3d());
  vtkView->initImage();
  vtkView->show();
}

void MainWindow::on_addNextVTKImage_clicked() {
  vtkView->setImage3d(applicationManager->getProcessedImage3d());
  vtkView->renderNewImage(std::make_tuple(1, 0.2, 0.2));
}

void MainWindow::on_saveCsvFile_clicked() {
  applicationManager->saveCSVFile(
      QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath())
          .toStdString());
}

void MainWindow::on_addToCsvFile_clicked() {
  applicationManager->addToCSVFile(
      { ui->ChooseOperation->currentText().toStdString(),
        ui->MorphologicalElementType->currentText().toStdString(),
        getDoubleText(ui->StructElementParam1->text()),
        getDoubleText(ui->StructElementParam2->text()),
        getDoubleText(ui->StructElementParam3->text()),
        ui->ProcessingWay->currentText().toStdString() });
}

void MainWindow::on_loadCsvFile_clicked() {
  applicationManager->loadCSVFile(
      QFileDialog::getOpenFileName(this, tr("Open CSV File"),
                                   QDir::currentPath(),
                                   tr("Csv Files(*.csv)")).toStdString());
}
