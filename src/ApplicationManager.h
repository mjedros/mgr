#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "Processing3dImage.h"
#include "csvFileUtils.h"
#include <QObject>
#include <QEvent>
namespace Mgr {
class Image3d;
enum SourceType : u_int8_t;

class ApplicationManager {
protected:
  std::shared_ptr<OpenCLManager> openCLManager;
  std::string sourceFilename;
  Image3d image3d;
  std::shared_ptr<Image3d> processedImage3d;
  Image3d image3dPrevious;
  CsvFile csvFile;
  bool processROI;
  ROI roi;

public:
  ApplicationManager(const std::shared_ptr<OpenCLManager> &openCLManagerPtr)
    : openCLManager(openCLManagerPtr), processROI(false) {}
  template <class T>
  void process(const OPERATION &operation, const std::string &structuralElement,
               const std::vector<float> &params) {
    cv::waitKey(1);
    image3dPrevious = *processedImage3d; // save image

    std::shared_ptr<ProcessingImage> img(
        new ProcessingImage(openCLManager, processROI));
    setROI(img);
    img->setStructuralElement(structuralElement, params);
    std::unique_ptr<Processing3dImage> processing3dImage;
    processing3dImage = std::unique_ptr<T>(new T);
    processing3dImage->process(processedImage3d, img, operation);
  }

  void init(const SourceType &source, const std::string &name);
  void initProcessedImage(const unsigned int &minumum = 100,
                          const unsigned int &maximum = 255);
  void setROI(const std::shared_ptr<ProcessingImage> &processingImage) {
    if (!processROI)
      return;
    processingImage->setROI(roi);
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

  const std::vector<std::vector<std::string>> &getOperationsVector() const {
    return csvFile.getOperationsVector();
  }
  const std::shared_ptr<Image3d> &getProcessedImage3d() const {
    return processedImage3d;
  }
};
}
#endif // APPLICATIONMANAGER_H
