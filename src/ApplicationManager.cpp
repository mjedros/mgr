#include "ApplicationManager.h"
#include <opencv2/opencv.hpp>
#include "ImageSource/SourceFactory.h"
#include "Normalization.h"

using namespace cv;

namespace Mgr {
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
}
