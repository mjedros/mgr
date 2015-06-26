#include <QtTest/QtTest>
#include "Logger.h"
#include "ProcessingImage.h"
#include "ProcessingImage3d.h"
#include "OpenCLManager.h"
#include "Image3d.h"
#include "include/Paths.h"

using namespace Mgr;
static Logger &logger = Logger::getInstance();
class ProcessingImagesTest : public QObject {
  Q_OBJECT
  OpenCLManager openCLManager;
  void CheckImagesEqual(cv::Mat one, cv::Mat two);
  void CheckImagesNotEqual(cv::Mat one, cv::Mat two);
  ProcessingImage img;
  ProcessingImage3d img3d;

  cv::Mat skeletonizeOpenCV(cv::Mat img);
  cv::Mat imageOriginal;

public:
  ProcessingImagesTest();
  void setToProcessAndBinarizeOriginalImage();
  Mgr::Image3d createImage3d(const int depth);

private Q_SLOTS:
  void init() { setToProcessAndBinarizeOriginalImage(); }
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
  void processMorphOperation();

  void image3dDilationEqual();
  void image3dDilationWithEllipsoid();
  void image3dDilationWithEllipsoidImage();
  void image3dDilationComparison();
};
ProcessingImagesTest::ProcessingImagesTest()
  : openCLManager(), img(openCLManager), img3d(openCLManager) {
  auto listPlatforms = openCLManager.listPlatforms();
  std::for_each(listPlatforms.begin(), listPlatforms.end(), [](auto &platform) {
    std::cout << (std::get<2>(platform)) << std::endl;
  });
  logger.printText("configuring");
  openCLManager.configure(std::string(KERNELS_DIR) + "Kernels.cl",
                          std::make_pair(0, 0));
  logger.printText("configured correctly!");

  imageOriginal = cv::Mat(220, 350, CV_8UC1);
  for (int i = 0; i < imageOriginal.cols; i++) {
    for (int j = 0; j < imageOriginal.rows; j++) {
      imageOriginal.at<uchar>(j, i) = i + j;
    }
  }
}
void ProcessingImagesTest::CheckImagesEqual(cv::Mat one, cv::Mat two) {
  QCOMPARE(*one.data, *two.data);
  QVERIFY(cv::countNonZero(one != two) == 0);
}
void ProcessingImagesTest::CheckImagesNotEqual(cv::Mat one, cv::Mat two) {
  QCOMPARE(*one.data, *two.data);
  QVERIFY(cv::countNonZero(one != two) != 0);
}
cv::Mat ProcessingImagesTest::skeletonizeOpenCV(cv::Mat img) {
  cv::Mat skel(img.size(), CV_8U, cv::Scalar(0));
  cv::Mat temp;
  cv::Mat eroded;

  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

  do {
    cv::erode(img, eroded, element);
    cv::dilate(eroded, temp, element);
    cv::subtract(img, temp, temp);
    cv::bitwise_or(skel, temp, skel);
    eroded.copyTo(img);
  } while (!cv::countNonZero(eroded) == 0);
  return skel;
}

void ProcessingImagesTest::setToProcessAndBinarizeOriginalImage() {
  img.setImageToProcess(imageOriginal.clone());
  img.binarize(127);
}

Image3d ProcessingImagesTest::createImage3d(const int depth) {
  cv::Mat image2d = img.getImage();
  Mgr::Image3d image3d(depth, img.getImage().clone());
  for (auto i = 0; i < depth; i++) {
    cv::Mat out = cv::Mat::zeros(image2d.size(), image2d.type());
    image2d(cv::Rect(0, i * 2, image2d.cols, image2d.rows - i * 2))
        .copyTo(out(cv::Rect(0, i * 2, image2d.cols, image2d.rows - i * 2)));
    image3d.setImageAtDepth(i, out);
  }

  return image3d;
}
void ProcessingImagesTest::EmptyImageTest() {
  ProcessingImage emptyImage(openCLManager);
  cv::Mat empty;
  QCOMPARE(empty.data, (emptyImage.getImage()).data);
}

void ProcessingImagesTest::SimpleImageTest() {
  cv::Mat eye = cv::Mat::eye({ 10, 10 }, 2);
  img.setImageToProcess(eye);

  QCOMPARE(*eye.data, *(img.getImage()).data);
  eye = cv::Mat::zeros({ 5, 5 }, 1);
  QVERIFY(*eye.data != *(img.getImage()).data);
}

void ProcessingImagesTest::binarizeTest() {
  cv::Mat image = imageOriginal.clone();
  cv::Mat thresholded;
  cv::threshold(image, thresholded, 127, 255, cv::THRESH_BINARY);
  img.setImageToProcess(image);
  img.binarize(127);
  CheckImagesEqual(thresholded, img.getImage());
}
void ProcessingImagesTest::binarizeTestTwoThresholds() {
  cv::Mat thresholded;
  cv::Mat image = imageOriginal.clone();
  cv::threshold(image, thresholded, 127, 255, cv::THRESH_BINARY);
  img.setImageToProcess(image);
  img.binarize(127, 255);
  CheckImagesEqual(thresholded, img.getImage());
}
void ProcessingImagesTest::DilateCrossTest() {
  cv::Mat dilated;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
  cv::dilate(img.getImage(), dilated, element);
  img.setStructuralElement("Cross", { 1, 1 });
  img.dilate();
  CheckImagesEqual(dilated, img.getImage());
}

void ProcessingImagesTest::ErodeCrossTest() {
  cv::Mat eroded;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
  cv::erode(img.getImage(), eroded, element);
  img.setStructuralElement("Cross", { 1, 1 });
  img.erode();
  CheckImagesEqual(eroded, img.getImage());
}

void ProcessingImagesTest::DilateRectangleTest() {
  cv::Mat dilated;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  cv::dilate(img.getImage(), dilated, element);
  img.setStructuralElement("Rectangle", { 1, 1 });
  img.dilate();
  CheckImagesEqual(dilated, img.getImage());
}

void ProcessingImagesTest::ErodeRectangleTest() {
  cv::Mat eroded;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  cv::erode(img.getImage(), eroded, element);
  img.setStructuralElement("Rectangle", { 1, 1 });
  img.erode();
  CheckImagesEqual(eroded, img.getImage());
}

void ProcessingImagesTest::skeletonizeTest() {
  cv::Mat skeletonized = skeletonizeOpenCV((img.getImage().clone()));
  img.setStructuralElement("Cross", { 1, 1 });
  img.skeletonize();
  CheckImagesEqual(skeletonized, img.getImage());
}

void ProcessingImagesTest::binarizefragmentOfmatTest() {
  cv::Mat image = imageOriginal.clone();
  imageOriginal.assignTo(image);
  cv::Mat imageFragment = image.colRange(0, image.cols / 2);
  cv::Mat thresholded;
  cv::threshold(imageFragment, thresholded, 127, 255, cv::THRESH_BINARY);
  img.setImageToProcess(imageFragment.clone());
  img.binarize(127);
  CheckImagesEqual(thresholded, img.getImage());
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
  RoiImage.binarize(80);
  cv::Mat roiImage = RoiImage.getImage();
  cv::Mat smallImage(
      roiImage(cv::Rect(lowx, lowy, highx - lowx, highy - lowy)));

  img.setImageToProcess(smallOrigin.clone());
  img.binarize(80);

  CheckImagesEqual(smallImage, img.getImage());
}

void ProcessingImagesTest::processMorphOperation() {
  cv::Mat dilated;
  cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
  cv::dilate(img.getImage(), dilated, element);
  img.setStructuralElement("Cross", { 1, 1 });
  img.setKernelWithOperation(OPERATION::DILATION);
  img.performMorphologicalOperation();
  CheckImagesEqual(dilated, img.getImage());
}

void ProcessingImagesTest::image3dDilationEqual() {
  const int depth = 12;
  Mgr::Image3d image3d = createImage3d(depth);
  img3d.set3dImageToProcess(image3d);
  img3d.setStructuralElement("Rectangle", { 1, 2, 0 });
  img.setStructuralElement("Rectangle", { 1, 2 });
  img.dilate();
  img3d.dilate();
  image3d.set3dImage(img3d.getImage());
  CheckImagesEqual(img.getImage(), image3d.getImageAtDepth(0));
  CheckImagesNotEqual(img.getImage(), image3d.getImageAtDepth(depth / 2));
}

void ProcessingImagesTest::image3dDilationWithEllipsoid() {
  const int depth = 5;
  Mgr::Image3d image3d = createImage3d(depth);
  img3d.set3dImageToProcess(image3d);
  img3d.setStructuralElement("Ellipse", { 2, 2, 2 });
  img3d.dilate();
  image3d.set3dImage(img3d.getImage());
}

void ProcessingImagesTest::image3dDilationWithEllipsoidImage() {
  const int depth = 5;
  Mgr::Image3d image3d = createImage3d(depth);
  img3d.set3dImageToProcess(image3d);
  img3d.setStructuralElement("EllipseImage", { 2, 2, 2 });
  img3d.dilate();
  image3d.set3dImage(img3d.getImage());
}

void ProcessingImagesTest::image3dDilationComparison() {
  ProcessingImage3d img3dCompare(openCLManager);
  const int depth = 5;
  Mgr::Image3d image3d = createImage3d(depth);
  img3d.set3dImageToProcess(image3d);
  img3dCompare.set3dImageToProcess(image3d);
  img3dCompare.setStructuralElement("Ellipse", { 3, 2, 3 });
  img3dCompare.dilate();
  img3d.setStructuralElement("EllipseImage", { 3, 2, 3 });
  img3d.dilate();
  CheckImagesEqual(img3dCompare.getImage(), img3d.getImage());
}

QTEST_APPLESS_MAIN(ProcessingImagesTest)

#include "tst_processingimagestest.moc"
