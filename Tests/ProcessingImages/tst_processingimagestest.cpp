#include <QtTest/QtTest>
#include "../../src/ProcessingImage.h"
#include "../../src/OpenCLManager.h"
#include "../src/include/Paths.h"

using namespace Mgr;
class ProcessingImagesTest : public QObject {
  Q_OBJECT
  std::shared_ptr<OpenCLManager> openCLManager;
  void CheckImagesEqual(cv::Mat one, cv::Mat two);
  std::unique_ptr<ProcessingImage> img;
  cv::Mat skeletonizeOpenCV(cv::Mat img);
  cv::Mat imageOriginal;

public:
  ProcessingImagesTest();
  cv::Mat setToProcessAndBinarizeOriginalImage();
private Q_SLOTS:
  void EmptyImageTest();
  void SimpleImageTest();
  void binarizeTest();
  void binarizeTestTwoThresholds();
  void DilateCrossTest();
  void ErodeCrossTest();
  void DilateRectangleTest();
  void ErodeRectangleTest();
  void skeletonizeTest();
  void binarizefragmentOfmatTest();
  void processROI();
};

void ProcessingImagesTest::CheckImagesEqual(cv::Mat one, cv::Mat two) {
  cv::Mat result;
  cv::compare(one, two, result, cv::CMP_EQ);
  int nz = cv::countNonZero(result);
  QVERIFY(nz = one.cols * one.rows);
}
cv::Mat ProcessingImagesTest::skeletonizeOpenCV(cv::Mat img) {
  cv::Mat skel(img.size(), CV_8U, cv::Scalar(0));
  cv::Mat temp;
  cv::Mat eroded;

  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

  bool done;
  do {
    cv::erode(img, eroded, element);
    cv::dilate(eroded, temp, element);
    cv::subtract(img, temp, temp);
    cv::bitwise_or(skel, temp, skel);
    eroded.copyTo(img);

    done = (cv::countNonZero(img) == 0);
  } while (!done);
  return skel;
}

cv::Mat ProcessingImagesTest::setToProcessAndBinarizeOriginalImage() {
  cv::Mat image = imageOriginal.clone();
  img->setImageToProcess(image);
  img->binarize(127);
  return image;
}
ProcessingImagesTest::ProcessingImagesTest()
  : openCLManager(new OpenCLManager) {
  auto listPlatforms = openCLManager->listPlatforms();
  std::for_each(listPlatforms.begin(), listPlatforms.end(),
                [this](std::tuple<int, int, std::string> &platform) {
    std::cout << (std::get<2>(platform)) << std::endl;
  });
  LOG("configuring")
  openCLManager->configure(std::string(KERNELS_DIR) + "Kernels.cl",
                           std::make_pair(0, 0));
  LOG("configured correctly!")
  img = std::unique_ptr<ProcessingImage>(new ProcessingImage(openCLManager));
  imageOriginal = cv::Mat(512, 203, CV_8UC1);
  for (int i = 0; i < imageOriginal.cols; i++) {
    for (int j = 0; j < imageOriginal.rows; j++) {
      imageOriginal.at<uchar>(j, i) = i;
    }
  }
}

void ProcessingImagesTest::EmptyImageTest() {
  cv::Mat img1 = img->getImage();
  cv::Mat empty;
  QCOMPARE(img1.data, empty.data);
}

void ProcessingImagesTest::SimpleImageTest() {
  cv::Mat eye = cv::Mat::eye({ 10, 10 }, 2);
  img->setImageToProcess(eye);

  QCOMPARE(*eye.data, *(img->getImage()).data);
  eye = cv::Mat::zeros({ 5, 5 }, 1);
  QVERIFY(*eye.data != *(img->getImage()).data);
}

void ProcessingImagesTest::binarizeTest() {
  cv::Mat image = imageOriginal.clone();
  cv::Mat thresholded;
  cv::threshold(image, thresholded, 127, 255, cv::THRESH_BINARY);
  img->setImageToProcess(image);
  img->binarize(127);
  CheckImagesEqual(thresholded, img->getImage());
}
void ProcessingImagesTest::binarizeTestTwoThresholds() {
  cv::Mat image = imageOriginal.clone();
  cv::Mat thresholded;
  cv::threshold(image, thresholded, 127, 255, cv::THRESH_BINARY);
  img->setImageToProcess(image);
  img->binarize(127, 255);
  CheckImagesEqual(thresholded, img->getImage());
}
void ProcessingImagesTest::DilateCrossTest() {
  setToProcessAndBinarizeOriginalImage();
  cv::Mat dilated;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
  cv::dilate(img->getImage(), dilated, element);
  img->setStructuralElement("Cross", { 1, 1 });
  img->dilate();
  CheckImagesEqual(dilated, img->getImage());
}

void ProcessingImagesTest::ErodeCrossTest() {
  setToProcessAndBinarizeOriginalImage();
  cv::Mat eroded;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
  cv::erode(img->getImage(), eroded, element);
  img->setStructuralElement("Cross", { 2, 2 });
  img->erode();
  CheckImagesEqual(eroded, img->getImage());
}

void ProcessingImagesTest::DilateRectangleTest() {
  setToProcessAndBinarizeOriginalImage();
  cv::Mat dilated;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  cv::dilate(img->getImage(), dilated, element);
  img->setStructuralElement("Rectangle", { 2, 2 });
  img->dilate();
  CheckImagesEqual(dilated, img->getImage());
}

void ProcessingImagesTest::ErodeRectangleTest() {
  setToProcessAndBinarizeOriginalImage();
  cv::Mat dilated;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
  cv::erode(img->getImage(), dilated, element);
  img->setStructuralElement("Rectangle", { 2, 2 });
  img->erode();
  CheckImagesEqual(dilated, img->getImage());
}

void ProcessingImagesTest::skeletonizeTest() {
  setToProcessAndBinarizeOriginalImage();
  cv::Mat skeletonized = skeletonizeOpenCV((img->getImage().clone()));
  img->setStructuralElement("Cross", { 1, 1 });
  img->skeletonize();
  CheckImagesEqual(skeletonized, img->getImage());
}

void ProcessingImagesTest::binarizefragmentOfmatTest() {
  cv::Mat image = imageOriginal.clone();
  imageOriginal.assignTo(image);
  cv::Mat imageFragment = image.colRange(0, image.cols / 2);
  cv::Mat thresholded;
  cv::threshold(imageFragment, thresholded, 127, 255, cv::THRESH_BINARY);
  img->setImageToProcess(imageFragment.clone());
  img->binarize(127);
  CheckImagesEqual(thresholded, img->getImage());
}

void ProcessingImagesTest::processROI() {
  cv::Mat image = imageOriginal.clone();
  int lowx = image.cols / 4;
  int lowy = image.rows / 4;
  int highx = image.cols / 2;
  int highy = image.rows / 2;
  cv::Mat smallOrigin(image(cv::Rect(lowx, lowy, highx - lowx, highy - lowy)));
  ROI roi =
      std::make_pair(std::make_pair(lowx, highx), std::make_pair(lowy, highy));
  ProcessingImage RoiImage(openCLManager, true);
  RoiImage.setROI(roi);
  RoiImage.setImageToProcess(image);
  RoiImage.binarize(127);
  cv::Mat roiImage = RoiImage.getImage();
  cv::Mat smallImage(
      roiImage(cv::Rect(lowx, lowy, highx - lowx, highy - lowy)));

  img->setImageToProcess(smallOrigin.clone());
  img->binarize(127);
  CheckImagesEqual(smallImage, img->getImage());
}

QTEST_APPLESS_MAIN(ProcessingImagesTest)

#include "tst_processingimagestest.moc"
