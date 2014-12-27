#include <QtTest>
#include "../../src/ProcessingImage.h"
#include "../../src/OpenCLManager.h"
#include "../src/include/Paths.h"

using namespace CLProcessingImage;
class ProcessingImagesTest : public QObject {
    Q_OBJECT
    std::shared_ptr<OpenCLManager> openCLManager;
    void CheckImagesEqual(cv::Mat one, cv::Mat two);
    std::unique_ptr<ProcessingImage> img;
    cv::Mat SkeletonizeOpenCV(cv::Mat img);
    cv::Mat image;

  public:
    ProcessingImagesTest();
  private Q_SLOTS:
    void EmptyImageTest();
    void SimpleImageTest();
    void BinarizeTest();
    void BinarizeTestTwoThresholds();
    void DilateCrossTest();
    void ErodeCrossTest();
    void DilateRectangleTest();
    void ErodeRectangleTest();
    void SkeletonizeTest();
};

void ProcessingImagesTest::CheckImagesEqual(cv::Mat one, cv::Mat two) {
    cv::Mat result;
    cv::compare(one, two, result, cv::CMP_EQ);
    int nz = cv::countNonZero(result);
    QVERIFY(nz == one.cols * one.rows);
}
cv::Mat ProcessingImagesTest::SkeletonizeOpenCV(cv::Mat img) {
    cv::Mat skel(img.size(), CV_8UC1, cv::Scalar(0));
    cv::Mat temp;
    cv::Mat eroded;

    cv::Mat element =
        cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

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

ProcessingImagesTest::ProcessingImagesTest()
    : openCLManager(new OpenCLManager) {
    openCLManager->Configure(std::string(KERNELS_DIR) + "Kernels.cl", std::make_pair(0, 0));
    img = std::unique_ptr<ProcessingImage>(new ProcessingImage(openCLManager));
    image = cv::Mat(512, 512, CV_8U);
    for (int i = 0; i < image.cols; i++) {
        for (int j = 0; j < image.rows; j++) {
            image.at<uchar>(j, i) = i;
        }
    }
}

void ProcessingImagesTest::EmptyImageTest() {
    cv::Mat img1 = img->GetImage();
    cv::Mat empty;
    QCOMPARE(img1.data, empty.data);
}

void ProcessingImagesTest::SimpleImageTest() {
    cv::Mat eye = cv::Mat::eye({ 10, 10 }, 2);
    img->SetImageToProcess(eye.clone());

    QCOMPARE(*eye.data, *(img->GetImage()).data);
    eye = cv::Mat::zeros({ 5, 5 }, 1);
    QVERIFY(*eye.data != *(img->GetImage()).data);
}

void ProcessingImagesTest::BinarizeTest() {
    img->SetImageToProcess(image.clone());
    img->Binarize(127);
    cv::Mat thresholded;
    cv::threshold(image, thresholded, 127, 255, cv::THRESH_BINARY);
    CheckImagesEqual(thresholded, img->GetImage());
}
void ProcessingImagesTest::BinarizeTestTwoThresholds() {
    img->SetImageToProcess(image.clone());
    img->Binarize(127, 255);
    cv::Mat thresholded;
    cv::threshold(image, thresholded, 127, 255, cv::THRESH_BINARY);
    CheckImagesEqual(thresholded, img->GetImage());
}
void ProcessingImagesTest::DilateCrossTest() {
    img->SetImageToProcess(image.clone());
    img->Binarize(127);
    cv::Mat dilated;
    cv::Mat element =
        cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::dilate(img->GetImage(), dilated, element);
    img->SetStructuralElement(CROSS, { 1, 1 });
    img->Dilate();
    CheckImagesEqual(dilated, img->GetImage());
}

void ProcessingImagesTest::ErodeCrossTest() {
    img->SetImageToProcess(image.clone());
    img->Binarize(127);
    cv::Mat eroded;
    cv::Mat element =
        cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
    cv::erode(img->GetImage(), eroded, element);
    img->SetStructuralElement(CROSS, { 2, 2 });
    img->Erode();
    CheckImagesEqual(eroded, img->GetImage());
}

void ProcessingImagesTest::DilateRectangleTest() {
    img->SetImageToProcess(image.clone());
    img->Binarize(127);
    cv::Mat dilated;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::dilate(img->GetImage(), dilated, element);
    img->SetStructuralElement(RECTANGLE, { 2, 2 });
    img->Dilate();
    CheckImagesEqual(dilated, img->GetImage());
}

void ProcessingImagesTest::ErodeRectangleTest() {
    img->SetImageToProcess(image.clone());
    img->Binarize(127);
    cv::Mat dilated;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::erode(img->GetImage(), dilated, element);
    img->SetStructuralElement(RECTANGLE, { 2, 2 });
    img->Erode();
    CheckImagesEqual(dilated, img->GetImage());
}

void ProcessingImagesTest::SkeletonizeTest() {
    img->SetImageToProcess(image.clone());
    img->Binarize(127);
    cv::Mat skeletonized = SkeletonizeOpenCV((img->GetImage()).clone());
    img->SetStructuralElement(CROSS, { 1, 1 });
    img->Skeletonize();
    CheckImagesEqual(skeletonized, img->GetImage());
}

QTEST_APPLESS_MAIN(ProcessingImagesTest)

#include "tst_processingimagestest.moc"
