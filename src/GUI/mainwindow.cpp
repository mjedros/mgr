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
    menu("File"), directory(QString("../Data/")), vtkView(this),
    openCLManager(), applicationManager(openCLManager),
    cameraProc(openCLManager, this) {
  ui->setupUi(this);
  setPlatformsList();
  menu.addAction("Open file to process", this, SLOT(openFileToProcess()));
  menu.addAction("Open directory to process", this, SLOT(openDirToProcess()));
  menu_bar.addMenu(&menu);
  ui->ChooseOperation->addItems({ "Dilation", "Erosion", "Opening", "Closing",
                                  "Contour", "Skeletonize", "Skeletonize2" });
  ui->MorphologicalElementType->addItems(
      { "Cross", "Rectangle", "Ellipse", "EllipseImage" });
  ui->ProcessingWay->addItems(
      { "Process depth", "Process columns", "Process rows", "Process in 3d" });
  ui->CameraDimension->addItems({ "2D", "3D" });
  ui->BackgroundSubstraction->addItems({ "None", "Delete first", "Mog2" });
  setMenuBar(&menu_bar);
}

MainWindow::~MainWindow() {}

void
MainWindow::on_ChoosePlatform_currentIndexChanged(const QString &description) {
  auto it = std::find_if(listPlatforms.begin(), listPlatforms.end(),
                         [&description](auto &platform) {
    return (description.toStdString() == std::get<2>(platform));
  });
  chosenDevice = std::make_pair(std::get<0>(*it), std::get<1>(*it));
}

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
    if (ui->BackgroundSubstraction->currentText().toStdString() == "Mog2")
      applicationManager.setBackgroundExtraction(BackgroundSubstr::MOG2);
    else if (ui->BackgroundSubstraction->currentText().toStdString() == "None")
      applicationManager.setBackgroundExtraction(BackgroundSubstr::NONE);
    else
      applicationManager.setBackgroundExtraction(BackgroundSubstr::FIRSTIMAGE);
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

void MainWindow::process() {
  auto processingValues = getProcessingValues();
  try {
    applicationManager.setProcessingROI(ui->processROI->isChecked());
    applicationManager.setProcessCv(ui->processCv->isChecked());
    applicationManager.process(std::get<0>(processingValues),
                               std::get<1>(processingValues),
                               std::get<2>(processingValues),
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

  processingThread.reset(new std::thread(&MainWindow::process, this));
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
  if (!openCLManager.isConfigured())
    openCLManager.configure(std::string(KERNELS_DIR) + "Kernels.cl",
                            chosenDevice);
  if (ui->File->isChecked() && filename.size() != 0)
    initImages(VideoFile, filename.toStdString());
  else if (ui->Directory->isChecked() && directory.size() != 0)
    initImages(DirectorySource, directory.toStdString() + "/");
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (aquisitionThread.joinable())
    aquisitionThread.join();
  cameraProc.stopProcessing();
  applicationManager.closeWindows();
  processedImage.close();
  originalImage.close();
  vtkView.close();
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
}

void MainWindow::on_ResetProcessed_clicked() { initBinaryImage(); }

void MainWindow::on_SaveImage_clicked() {
  applicationManager.saveOriginalImage(
      QFileDialog::getExistingDirectory(this).toStdString());
}

void MainWindow::on_vtkViewButton_clicked() {
  vtkView.setImage3d(applicationManager.getProcessedImage3d());
  vtkView.initImage();
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
    getDoubleText(ui->StructElementParam3->text())
  };
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

void processCsvSequenceThread(ApplicationManagerGUI *appManager,
                              QLabel *processingLabel,
                              const std::string &operationWay) {
  appManager->processCsvSequence(operationWay);
  processingLabel->setText("Done");
}

void MainWindow::on_processCsvSequence_clicked() {
  logger.printFancyLine("Starting sequence");
  applicationManager.setProcessCv(ui->processCv->isChecked());
  applicationManager.setProcessingROI(ui->processROI->isChecked());
  ui->ProcessingProgress->setEnabled(true);
  ui->ProcessingProgress->setText("In progress");
  cv::waitKey(1);
  processingThread.reset(new std::thread(
      processCsvSequenceThread, &applicationManager, ui->ProcessingProgress,
      ui->ProcessingWay->currentText().toStdString()));
  processingThread->detach();
  logger.printFancyLine("");
}

void MainWindow::on_CloseWindows_clicked() {
  cameraProc.stopProcessing();
  applicationManager.closeWindows();
}

std::tuple<std::string, std::string, const std::vector<float>>
MainWindow::getProcessingValues() {

  const std::vector<float> StructElemParams = {
    static_cast<float>(ui->StructElementParam1->value()),
    static_cast<float>(ui->StructElementParam2->value()),
    static_cast<float>(ui->StructElementParam3->value())
  };
  return std::make_tuple(
      ui->ChooseOperation->currentText().toStdString(),
      ui->MorphologicalElementType->currentText().toStdString(),
      StructElemParams);
}

void MainWindow::on_Revert_clicked() {
  applicationManager.revertLastOperation();
}
void MainWindow::startAquisition() {
  if (!openCLManager.isConfigured())
    openCLManager.configure(std::string(KERNELS_DIR) + "Kernels.cl",
                            chosenDevice);

  auto values = getProcessingValues();
  cameraProc.setProcessing(std::get<0>(values), std::get<1>(values),
                           std::get<2>(values), ui->lowLewel->value(),
                           ui->highLevel->value());
  if (ui->CameraDimension->currentText() == "3D")
    cameraProc.process3dImages();
  else
    cameraProc.process2dImages();
}

void MainWindow::drawObject(cv::Mat img) {
  originalImage.draw(img);
  originalImage.update();
}

void MainWindow::drawProcessed(cv::Mat img) {
  processedImage.draw(img);
  processedImage.update();
}

void MainWindow::drawVtkImage(const std::shared_ptr<Mgr::Image3d> &image3d) {
  vtkView.setImage3d(image3d);
  vtkViewThread = std::thread(&VTKView::initImage, &vtkView);
  vtkViewThread.detach();
}
void MainWindow::on_CameraAquisition_clicked() {
  aquisitionThread = std::thread(&MainWindow::startAquisition, this);
  aquisitionThread.detach();
}

void MainWindow::on_UpdateOperation_clicked() {
  auto values = getProcessingValues();
  cameraProc.setProcessing(std::get<0>(values), std::get<1>(values),
                           std::get<2>(values), ui->lowLewel->value(),
                           ui->highLevel->value());
}

void MainWindow::on_deleteFromCsvFile_clicked() {
  if (ui->csvOperations->currentIndex().row() == -1)
    return;
  applicationManager.deleteOperation(ui->csvOperations->currentIndex().row());
  updateCSVOperations();
  ui->csvOperations->scrollToTop();
}
