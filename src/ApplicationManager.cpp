#include "ApplicationManager.h"

#include "ImageSource/SourceFactory.h"
#include "Logger.h"
#include "Normalization.h"
#include "Processing3dImage.h"
#include <opencv2/opencv.hpp>
using namespace cv;

namespace Mgr {

std::map<std::string, OPERATION> OperationMap = {
  { "Dilation", OPERATION::DILATION },
  { "Erosion", OPERATION::EROSION },
  { "Contour", OPERATION::CONTOUR },
  { "Skeletonize", OPERATION::SKELETONIZATION }
};

static Logger &logger = Logger::getInstance();
template <class T, class I>
void ApplicationManager::process(const OPERATION &operation,
                                 const std::string &structuralElement,
                                 const std::vector<float> &params) {
  cv::waitKey(1);
  image3dPrevious = *processedImage3d; // save image

  I img(openCLManager, processROI);
  T processing3dImage;
  if (!isROISizeValid(processing3dImage.getImageSize(*processedImage3d)))
    throw new std::string("Wrong ROI size");
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

bool ApplicationManager::isROISizeValid(std::pair<int, int> imageSize) {
  if (!processROI)
    return true;
  return !(roi.first.second > imageSize.first ||
           roi.second.second > imageSize.second || roi.first.second == 0 ||
           roi.second.second == 0);
}

void ApplicationManager::init(const SourceType &source, const string &name) {
  std::unique_ptr<IImageSource> imageSource =
      SourceFactory::GetImageSource(source, name);
  imageSource->Start();
  std::vector<cv::Mat> matVector;

  for (Mat im = imageSource->Get(); !im.empty(); im = imageSource->Get()) {
    cv::cvtColor(im, im, CV_BGR2GRAY);
    matVector.push_back(im);
  }

  image3d = Image3d(matVector.size(), *matVector.begin());
  for (auto it = matVector.begin(); it != matVector.end(); it++) {
    image3d.setImageAtDepth(std::distance(matVector.begin(), it), *it);
  }
}

void ApplicationManager::initProcessedImage(const unsigned int &minumum,
                                            const unsigned int &maximum) {
  logger.printLine("Init binary image");
  processROI = false;
  processedImage3d.reset(
      new Image3d(image3d.getDepth(), image3d.getImageAtDepth(0)));
  ProcessingImage img(openCLManager, processROI);
  for (auto i = 0; i < processedImage3d->getDepth(); i++) {
    img.setImageToProcess(image3d.getImageAtDepth(i).clone());
    img.binarize(minumum, maximum);
    processedImage3d->setImageAtDepth(i, img.getImage());
  }
  cv::imwrite("/home/michal/data.jpg", processedImage3d->get3dMatImage());
}
void ApplicationManager::normalizeOriginalImage() { normalize(image3d); }

void ApplicationManager::saveOriginalImage(const std::string &filename) {
  saveMovie(image3d, filename);
}

void ApplicationManager::saveProcessedImage(const std::string &filename) {
  saveMovie(*processedImage3d, filename);
}

void ApplicationManager::processCsvSequence() {
  const auto &operVect = getOperationsVector();
  for (auto &tokens : operVect) {
    if (tokens[0] == "Binarize") {
      setProcessingROI(false);
      initProcessedImage(std::stoi(tokens[1]), std::stoi(tokens[2]));
    } else {
      processROI = (tokens[6] == "1" ? true : false);
      if (processROI) {
        roi = std::make_pair(
            std::make_pair(std::stoi(tokens[7]), std::stoi(tokens[8])),
            std::make_pair(std::stoi(tokens[9]), std::stoi(tokens[10])));
      }
      process(
          tokens[0], tokens[1],
          { std::stof(tokens[2]), std::stof(tokens[3]), std::stof(tokens[4]) },
          tokens[5]);
    }
  }
}
void ApplicationManager::process(const std::string &operationString,
                                 const std::string &MorphElementType,
                                 const std::vector<float> StructElemParams,
                                 const std::string &operationWay) {
  logger.printText("Performing " + operationString);
  logger.printText(", " + operationWay + ", " + MorphElementType);
  logger.printProcessingROI(processROI);
  const OPERATION &operation = OperationMap[operationString];

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
}
