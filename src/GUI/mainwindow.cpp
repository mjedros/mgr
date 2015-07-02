#include "GUI/mainwindow.h"

#include "ui_mainwindow.h"
#include "include/Paths.h"
#include "GUI/vtkview.h"
#include "ContinuousProcessingMananger.h"
#include "Image3d.h"
#include "ImageSource/SourceFactory.h"
#include "Logger.h"
#include "Processing3dImage.h"

#include <iostream>
#include <QFileDialog>

using namespace Mgr;

namespace {
inline std::string getDoubleText(const QString &text) {
  std::string doubleText = text.toStdString();
  doubleText.replace(doubleText.find(','), 1, ".");
  return doubleText;
}
}
static Logger &logger = Logger::getInstance();

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow), csvOperationsModel(this),
    menu("File"), directory(QString("../Data/")), openCLManager(),
    applicationManager(openCLManager), cameraProc(openCLManager, this) {
  ui->setupUi(this);
  setPlatformsList();
  menu.addAction("Open file to process", this, SLOT(openFileToProcess()));
  menu.addAction("Open directory to process", this, SLOT(openDirToProcess()));
  menu_bar.addMenu(&menu);
  ui->ChooseOperation->addItems(
      { "Dilation", "Erosion", "Contour", "Skeletonize" });
  ui->MorphologicalElementType->addItems(
      { "Cross", "Rectangle", "Ellipse", "EllipseImage" });
  ui->ProcessingWay->addItems(
      { "Process depth", "Process columns", "Process rows", "Process in 3d" });
  setMenuBar(&menu_bar);
}

MainWindow::~MainWindow() {}

void
MainWindow::on_ChoosePlatform_currentIndexChanged(const QString &description) {
  auto it = std::find_if(listPlatforms.begin(), listPlatforms.end(),
                         [&](auto &platform) {
    return (description.toStdString() == std::get<2>(platform));
  });
  chosenDevice = std::make_pair(std::get<0>(*it), std::get<1>(*it));
}
// void
// MainWindow::on_ChooseOperation_currentIndexChanged(const QString &operation)
// {

//}

void MainWindow::setPlatformsList() {
  listPlatforms = openCLManager.listPlatforms();
  std::for_each(listPlatforms.begin(), listPlatforms.end(),
                [this](auto &platform) {
    ui->ChoosePlatform->addItem(QString::fromStdString(std::get<2>(platform)));
  });
}

void MainWindow::initImages(const Mgr::SourceType &source,
                            const std::string &name) {
  try {
    applicationManager.init(source, name);
  } catch (std::string &err) {
    logger.printLine(err);
    return;
  } catch (...) {
    logger.printLine("Unknown init error!");
    return;
  }

  initBinaryImage();
  ui->ImagesLoaded->setText("Images Loaded");
  ui->Process->setEnabled(true);
  ui->ShowWindows->setEnabled(true);
  ui->CloseWindows->setEnabled(true);
  ui->ResetProcessed->setEnabled(true);
  ui->Normalize->setEnabled(true);
  ui->SaveImage->setEnabled(true);
  ui->Revert->setEnabled(true);
}

void MainWindow::updateCSVOperations() {
  QStringList List;
  auto operationsVector = applicationManager.getOperationsVector();
  QString parameters;
  for (auto &tokens : operationsVector) {
    parameters.clear();
    for (auto &params : tokens)
      parameters += QString::fromStdString(params) + " ";
    List << parameters;
  }
  csvOperationsModel.setStringList(List);
  ui->csvOperations->setModel(&csvOperationsModel);
}

void MainWindow::process(const std::vector<float> StructElemParams,
                         const std::string MorphElementType) {
  try {
    applicationManager.setProcessingROI(ui->processROI->isChecked());
    applicationManager.process(ui->ChooseOperation->currentText().toStdString(),
                               MorphElementType, StructElemParams,
                               ui->ProcessingWay->currentText().toStdString());
    ui->Process->show();

  } catch (std::string *error) {
    ui->ProcessingProgress->setText(QString::fromStdString(*error));
  }
  { ui->ProcessingProgress->setText("Done"); }
}

void MainWindow::on_Process_clicked() {
  ui->ProcessingProgress->setEnabled(true);
  ui->ProcessingProgress->setText("In progress");
  cv::waitKey(1);

  const std::string MorphElementType =
      ui->MorphologicalElementType->currentText().toStdString();
  const std::vector<float> StructElemParams = {
    static_cast<float>(ui->StructElementParam1->value()),
    static_cast<float>(ui->StructElementParam2->value()),
    static_cast<float>(ui->StructElementParam3->value())
  };

  processingThread.reset(new std::thread(&MainWindow::process, this,
                                         StructElemParams, MorphElementType));
  processingThread->detach();
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
  openCLManager.configure(std::string(KERNELS_DIR) + "Kernels.cl",
                          chosenDevice);
  if (ui->File->isChecked() && filename.size() != 0)
    initImages(VideoFile, filename.toStdString());
  else if (ui->Directory->isChecked() && directory.size() != 0)
    initImages(DirectorySource, directory.toStdString() + "/");
}

void MainWindow::closeEvent(QCloseEvent *event) {
  cameraProc.stopProcessing();
  applicationManager.closeWindows();
  processedImage.close();
  originalImage.close();
  vtkView.reset();
  QMainWindow::closeEvent(event);
}

void MainWindow::on_ShowWindows_clicked() { applicationManager.showImages(); }

void MainWindow::on_Normalize_clicked() {
  applicationManager.normalizeOriginalImage();
  initBinaryImage();
}

void MainWindow::initBinaryImage() {
  applicationManager.initProcessedImage(ui->lowLewel->value(),
                                        ui->highLevel->value());
  applicationManager.addToCSVFile({ "Binarize",
                                    std::to_string(ui->lowLewel->value()),
                                    std::to_string(ui->highLevel->value()) });
  updateCSVOperations();
}

void MainWindow::on_ResetProcessed_clicked() { initBinaryImage(); }

void MainWindow::on_SaveImage_clicked() {
  applicationManager.saveOriginalImage(
      QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath())
          .toStdString());
}

void MainWindow::on_vtkViewButton_clicked() {
  vtkView.reset(new VTKView());
  vtkView->setImage3d(applicationManager.getProcessedImage3d());
  vtkView->initImage();
  vtkView->show();
}

void MainWindow::on_addNextVTKImage_clicked() {
  vtkView->setImage3d(applicationManager.getProcessedImage3d());
  vtkView->renderNewImage(
      std::make_tuple((double)ui->RedColor->value() / 255.0,
                      (double)ui->GreenColor->value() / 255.0,
                      (double)ui->BlueColor->value() / 255.0));
}

void MainWindow::on_saveCsvFile_clicked() {
  applicationManager.saveCSVFile(
      QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath())
          .toStdString());
}

void MainWindow::on_addToCsvFile_clicked() {
  std::vector<std::string> operationsVector = {
    ui->ChooseOperation->currentText().toStdString(),
    ui->MorphologicalElementType->currentText().toStdString(),
    getDoubleText(ui->StructElementParam1->text()),
    getDoubleText(ui->StructElementParam2->text()),
    getDoubleText(ui->StructElementParam3->text()),
    ui->ProcessingWay->currentText().toStdString(),
    ui->processROI->isChecked() ? "1" : "0"
  };
  if (ui->processROI->isChecked()) {
    const ROI &roi = applicationManager.getROI();

    std::vector<std::string> roiString = { std::to_string(roi.first.first),
                                           std::to_string(roi.first.second),
                                           std::to_string(roi.second.first),
                                           std::to_string(roi.second.second) };
    operationsVector.insert(operationsVector.end(), roiString.begin(),
                            roiString.end());
  }
  applicationManager.addToCSVFile(operationsVector);
  updateCSVOperations();
}

void MainWindow::on_loadCsvFile_clicked() {
  applicationManager.loadCSVFile(
      QFileDialog::getOpenFileName(this, tr("Open CSV File"),
                                   QDir::currentPath(),
                                   tr("Csv Files(*.csv)")).toStdString());
  updateCSVOperations();
}

void MainWindow::on_processCsvSequence_clicked() {
  logger.printFancyLine("Starting sequence");
  applicationManager.processCsvSequence();
  logger.printLine("Done processing sequence");
  logger.printFancyLine("");
}

void MainWindow::on_CloseWindows_clicked() {
  cameraProc.stopProcessing();
  applicationManager.closeWindows();
}

void MainWindow::on_Revert_clicked() {
  applicationManager.revertLastOperation();
}
void MainWindow::startAquisition() {
  openCLManager.configure(std::string(KERNELS_DIR) + "Kernels.cl",
                          chosenDevice);
  const std::string MorphElementType =
      ui->MorphologicalElementType->currentText().toStdString();
  const std::vector<float> StructElemParams = {
    static_cast<float>(ui->StructElementParam1->value()),
    static_cast<float>(ui->StructElementParam2->value()),
    static_cast<float>(ui->StructElementParam3->value())
  };
  // TODO remove copypaiste
  cameraProc.setProcessing(ui->ChooseOperation->currentText().toStdString(),
                           MorphElementType, StructElemParams);
  cameraProc.process3dImages();
}

void MainWindow::drawObject(cv::Mat img) {
  originalImage.draw(img);
  originalImage.update();
}

void MainWindow::drawProcessed(cv::Mat img) {
  processedImage.draw(img);
  processedImage.update();
}
void MainWindow::on_CameraAquisition_clicked() {
  aquisitionThread = std::thread(&MainWindow::startAquisition, this);
  aquisitionThread.detach();
}

void MainWindow::on_deleteFromCsvFile_clicked() {
  if (ui->csvOperations->currentIndex().row() == -1)
    return;
  applicationManager.deleteOperation(ui->csvOperations->currentIndex().row());
  updateCSVOperations();
}
