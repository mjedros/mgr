#include "ApplicationManager.h"

#include "ImageSource/SourceFactory.h"
#include "Logger.h"
#include "Normalization.h"
#include "Processing3dImage.h"
#include <opencv2/opencv.hpp>
using namespace cv;

namespace Mgr {

static Logger &logger = Logger::getInstance();
template <class T>
void ApplicationManager::process(const OPERATION &operation,
                                 const std::string &structuralElement,
                                 const std::vector<float> &params) {
  cv::waitKey(1);
  image3dPrevious = *processedImage3d; // save image

  ProcessingImage img(openCLManager, processROI);
  T processing3dImage;
  if (!isROISizeValid(processing3dImage.getImageSize(processedImage3d)))
    throw new std::string("Wrong ROI size");
  setROI(img);
  img.setStructuralElement(structuralElement, params);

  processing3dImage.process(processedImage3d, img, operation);
}

void saveMovie(const Image3d &image, const std::string &filename) {

  VideoWriter videowriter(filename, CV_FOURCC('D', 'I', 'V', 'X'), 300,
                          cv::Size(image.getRows(), image.getCols()), false);
  if (!videowriter.isOpened()) {
    std::cout << "Could not open the output video for write " << std::endl;
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
  processROI = false;
  processedImage3d.reset(
      new Image3d(image3d.getDepth(), image3d.getImageAtDepth(0)));
  std::shared_ptr<ProcessingImage> img(
      new ProcessingImage(openCLManager, processROI));
  for (auto i = 0; i < processedImage3d->getDepth(); i++) {
    img->setImageToProcess(image3d.getImageAtDepth(i).clone());
    img->binarize(minumum, maximum);
    processedImage3d->setImageAtDepth(i, img->getImage());
  }
  std::cout << getAvarage() << std::endl;
  csvFile.addOperations(
      { "Binarize", std::to_string(minumum), std::to_string(maximum) });
}
void ApplicationManager::normalizeOriginalImage() { normalize(image3d); }

void ApplicationManager::saveOriginalImage(const std::string &filename) {
  saveMovie(image3d, filename);
}

void ApplicationManager::saveProcessedImage(const std::string &filename) {
  saveMovie(*processedImage3d, filename);
}
template void
ApplicationManager::process<ProcessCols>(const OPERATION &operation,
                                         const std::string &structuralElement,
                                         const std::vector<float> &params);
template void
ApplicationManager::process<ProcessDepth>(const OPERATION &operation,
                                          const std::string &structuralElement,
                                          const std::vector<float> &params);
}
