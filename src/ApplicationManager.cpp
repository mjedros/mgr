#include "ApplicationManager.h"

#include "ImageSource/SourceFactory.h"
#include "Logger.h"
#include "Normalization.h"
#include "Processing3dImage.h"
#include <opencv2/opencv.hpp>
#include <mutex>
using namespace cv;

namespace Mgr {

static Logger &logger = Logger::getInstance();
template <class T, class I>
void ApplicationManager::process(const OPERATION &operation,
                                 const std::string &structuralElement,
                                 const std::vector<float> &params) {
  image3dPrevious = *processedImage3d; // save image
  I img(openCLManager, processROI);
  img.setProcessCV(processOpenCv);
  T processing3dImage;
  if (!isROISizeValid(processing3dImage.getImageSize(*processedImage3d))) {
    logger.printLine("Wrong ROI size");
    const auto imageSize = processing3dImage.getImageSize(*processedImage3d);
    std::string sizes = std::to_string(roi.first.second) + " , " +
                        std::to_string(imageSize.first) + "," +
                        std::to_string(roi.second.second) + "," +
                        std::to_string(imageSize.second);
    logger.printLine(sizes);
    throw new std::string("Wrong ROI size");
  }
  setROI(img);
  img.setStructuralElement(structuralElement, params);
  logger.resetTimer();
  processing3dImage.process(*processedImage3d, img, operation);
  logger.printAvarageTime();
}

void saveMovie(const Image3d &image, const std::string &filename) {

  VideoWriter videowriter(filename, CV_FOURCC('D', 'I', 'V', 'X'), 300,
                          cv::Size(image.getRows(), image.getCols()), false);
  if (!videowriter.isOpened()) {
    logger.printLine("Could not open the output video for write ");
    return;
  }

  for (int j = 0; j < image.getDepth(); j++) {
    videowriter << image.getImageAtDepth(j);
  }
}
void saveFiles(const Image3d &image, const std::string &directory) {
  std::cout << directory;
  const std::string dirAndName = directory + "/fileprocessed";
  for (int j = 0; j < image.getDepth(); j++) {
    cv::imwrite(dirAndName + std::to_string(j) + ".jpg",
                image.getImageAtDepth(j));
  }
}
bool ApplicationManager::isROISizeValid(std::pair<int, int> imageSize) {
  if (!processROI)
    return true;
  return !(roi.first.second > imageSize.first ||
           roi.second.second > imageSize.second || roi.first.second == 0 ||
           roi.second.second == 0);
}

void ApplicationManager::init(const SourceType &source, const string &name) {
  image3d.clear();
  if (processedImage3d)
    processedImage3d->clear();
  std::unique_ptr<IImageSource> imageSource =
      SourceFactory::GetImageSource(source, name);
  int i = 0;
  imageSource->Start();
  std::vector<cv::Mat> matVector;
  bool first = true;
  Mat firstIm;
  for (Mat im = imageSource->Get(); !im.empty(); im = imageSource->Get(), ++i) {
    if (backgroundSubstr == BackgroundSubstr::FIRSTIMAGE) {
      if (first) {
        firstIm = im.clone();
        first = false;
        continue;
      }
      im = firstIm - im;
    }
    cv::cvtColor(im, im, CV_BGR2GRAY);
    matVector.push_back(im);
  }
  logger.printLine("Done adding to vector");
  image3d = Image3d(matVector.size(), *matVector.begin());
  for (auto it = matVector.begin(); it != matVector.end(); it++) {
    image3d.setImageAtDepth(std::distance(matVector.begin(), it), *it);
  }
}

void ApplicationManager::initProcessedImage(const unsigned int &minumum,
                                            const unsigned int &maximum) {
  processROI = false;
  processedImage3d.reset(
      new Image3d(image3d.getDepth(), image3d.getImageAtDepth(0)));
  ProcessingImage img(openCLManager, processROI);
  cv::BackgroundSubtractorMOG2 bg(6, 10, false);
  bg.set("nmixtures", 3);
  for (auto i = 0; i < processedImage3d->getDepth(); i++) {
    Mat fore = image3d.getImageAtDepth(i);
    if (backgroundSubstr == BackgroundSubstr::MOG2)
      bg.operator()(fore, fore);
    img.setImageToProcess(fore);
    img.binarize(minumum, maximum);
    processedImage3d->setImageAtDepth(i, img.getImage());
  }
}
void ApplicationManager::normalizeOriginalImage() { normalize(image3d); }

void ApplicationManager::saveOriginalImage(const std::string &filename) {
  saveFiles(*processedImage3d, filename);
}

void ApplicationManager::saveProcessedImage(const std::string &filename) {
  saveMovie(*processedImage3d, filename);
}

void ApplicationManager::processCsvSequence(const std::string &operationWay) {
  if (operationWay == "Process columns")
    processCsvSequence<ProcessCols>();
  else if (operationWay == "Process depth")
    processCsvSequence<ProcessDepth>();
  else if (operationWay == "Process rows")
    processCsvSequence<ProcessRows>();
  else
    processCsvSequence<ProcessDepthIn3D, ProcessingImage3d>();
  logger.printLine("Done processing sequence");
}
void ApplicationManager::process(const std::string &operationString,
                                 const std::string &MorphElementType,
                                 const std::vector<float> StructElemParams,
                                 const std::string &operationWay) {
  logger.printProcessing(operationString, MorphElementType, StructElemParams,
                         operationWay, processROI);
  if (processOpenCv)
    logger.printLine("Opencv processing");

  const OPERATION &operation = ProcessingImage::OperationMap[operationString];

  if (operationWay == "Process columns")
    process<ProcessCols>(operation, MorphElementType, StructElemParams);
  else if (operationWay == "Process depth")
    process<ProcessDepth>(operation, MorphElementType, StructElemParams);
  else if (operationWay == "Process rows")
    process<ProcessRows>(operation, MorphElementType, StructElemParams);
  else
    process<ProcessDepthIn3D, ProcessingImage3d>(operation, MorphElementType,
                                                 StructElemParams);
}
template <class T, class I> void ApplicationManager::processCsvSequence() {
  image3dPrevious = *processedImage3d; // save image
  I img(openCLManager, processROI);
  img.setProcessCV(processOpenCv);
  T processing3dImage;
  setROI(img);
  logger.resetTimer();
  processing3dImage.processSequence(*processedImage3d, img,
                                    getOperationsVector());
  logger.printAvarageTime();
}
template void
ApplicationManager::process<ProcessCols>(const OPERATION &operation,
                                         const std::string &structuralElement,
                                         const std::vector<float> &params);
template void
ApplicationManager::process<ProcessDepth>(const OPERATION &operation,
                                          const std::string &structuralElement,
                                          const std::vector<float> &params);
template void
ApplicationManager::process<ProcessRows>(const OPERATION &operation,
                                         const std::string &structuralElement,
                                         const std::vector<float> &params);
template void ApplicationManager::process<ProcessDepthIn3D, ProcessingImage3d>(
    const OPERATION &operation, const std::string &structuralElement,
    const std::vector<float> &params);
template void ApplicationManager::processCsvSequence<ProcessCols>();
template void ApplicationManager::processCsvSequence<ProcessDepth>();
template void ApplicationManager::processCsvSequence<ProcessRows>();
template void
ApplicationManager::processCsvSequence<ProcessDepthIn3D, ProcessingImage3d>();
}
