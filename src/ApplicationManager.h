#pragma once

#include "Image3d.h"
#include "ProcessingImage.h"
#include "ProcessingImage3d.h"
#include "OpenCLManager.h"
#include "csvFileUtils.h"
#include <QObject>
#include <QEvent>
#include <queue>

namespace Mgr {
class Image3d;
enum SourceType : u_int8_t;
enum class OPERATION : u_int8_t;
class Image3d;

class ApplicationManager {
protected:
  OpenCLManager &openCLManager;
  std::string sourceFilename;
  Image3d image3d;
  std::shared_ptr<Image3d> processedImage3d;
  Image3d image3dPrevious;
  CsvFile csvFile;
  bool processROI;
  ROI roi;
  bool isROISizeValid(std::pair<int, int> imageSize);
  std::vector<cv::Mat> imagesVector;
  std::queue<std::vector<cv::Mat *>> portionsQueue;

public:
  ApplicationManager(OpenCLManager &openCLManagerRef)
    : openCLManager(openCLManagerRef), processROI(false) {}

  template <class T, class I = ProcessingImage>
  void process(const OPERATION &operation, const std::string &structuralElement,
               const std::vector<float> &params);

  void init(const SourceType &source, const std::string &name);
  void startCameraAquisition();
  void initProcessedImage(const unsigned int &minumum = 100,
                          const unsigned int &maximum = 255);
  template <class I = ProcessingImage> void setROI(I &processingImage) {
    if (!processROI)
      return;
    processingImage.setROI(roi);
  }
  void setROI(const ROI &newROI) { roi = newROI; }
  const ROI &getROI() { return roi; }
  void setProcessingROI(bool processROINew) { processROI = processROINew; }

  void normalizeOriginalImage();
  void saveOriginalImage(const std::string &filename);
  void saveProcessedImage(const std::string &filename);
  void saveCSVFile(const std::string &filename) { csvFile.saveFile(filename); }
  void loadCSVFile(const std::string &filename) { csvFile.loadFile(filename); }
  void revertLastOperation() {
    processedImage3d.reset(new Image3d(image3dPrevious));
  }
  void addToCSVFile(const std::vector<std::string> &operationsVector) {
    csvFile.addOperations(operationsVector);
  }
  void deleteOperation(const int &number) { csvFile.deleteOperation(number); }
  void processCsvSequence();

  const std::vector<std::vector<std::string>> &getOperationsVector() const {
    return csvFile.getOperationsVector();
  }
  const std::shared_ptr<Image3d> &getProcessedImage3d() const {
    return processedImage3d;
  }
  void process(const std::string &operationString,
               const std::string &MorphElementType,
               const std::vector<float> StructElemParams,
               const std::string &operationWay);
};
}
