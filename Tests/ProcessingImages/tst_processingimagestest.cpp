#include <QString>
#include <QtTest>
#include "../../src/ProcessingImage.h"
#include "../../src/OpenCLManager.h"
class ProcessingImagesTest : public QObject
{
   Q_OBJECT
   std::shared_ptr<OpenCLManager> openCLManager;
   void CheckImagesEqual(cv::Mat one,cv::Mat two);
 public:
   ProcessingImagesTest();
 private Q_SLOTS:
   void EmptyImageTest();
   void SimpleImageTest();
   void BinarizeTest();
   void DilateCrossTest();
   void ErodeCrossTest();
};

void ProcessingImagesTest::CheckImagesEqual(cv::Mat one, cv::Mat two)
{
    cv::Mat result;
    cv::compare(one,two,result,cv::CMP_EQ);
    int nz = cv::countNonZero(result);
    QVERIFY(nz == one.cols*one.rows);
}

ProcessingImagesTest::ProcessingImagesTest() : openCLManager(new OpenCLManager) {}

void ProcessingImagesTest::EmptyImageTest()
{
   std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
   cv::Mat img1 = img->GetImage();
   cv::Mat empty;
   QCOMPARE(img1.data, empty.data);
}

void ProcessingImagesTest::SimpleImageTest()
{

   std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
   cv::Mat eye = cv::Mat::eye({10, 10}, 2);
   img->SetImageToProcess(eye.clone());

   QCOMPARE(*eye.data, *(img->GetImage()).data);
   eye = cv::Mat::zeros({5, 5}, 1);
   QVERIFY(*eye.data != *(img->GetImage()).data);
}

void ProcessingImagesTest::BinarizeTest()
{
    openCLManager->Configure("../../Kernels/Kernels.cl", std::make_pair(0, 0));
    std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
    cv::Mat image = cv::imread("../../Data/napis.jpg");
    cv::cvtColor(image, image, CV_BGR2GRAY);
    img->SetImageToProcess(image.clone());
    img->Threshold(0.5);
    cv::Mat thresholded;
    cv::threshold(image,thresholded,127,255,cv::THRESH_BINARY_INV);
    CheckImagesEqual(thresholded,img->GetImage());
}

void ProcessingImagesTest::DilateCrossTest()
{
    openCLManager->Configure("../../Kernels/Kernels.cl", std::make_pair(0, 0));
    std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
    cv::Mat image = cv::imread("../../Data/napis.jpg");
    cv::cvtColor(image, image, CV_BGR2GRAY);
    img->SetImageToProcess(image.clone());
    img->Threshold(0.5);
    cv::Mat dilated;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::dilate(img->GetImage(), dilated, element);
    img->SetStructuralElement(cross,{1,1});
    img->Dilate();
    CheckImagesEqual(dilated, img->GetImage());
}

void ProcessingImagesTest::ErodeCrossTest()
{
    openCLManager->Configure("../../Kernels/Kernels.cl", std::make_pair(0, 0));
    std::unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
    cv::Mat image = cv::imread("../../Data/napis.jpg");
    cv::cvtColor(image, image, CV_BGR2GRAY);
    img->SetImageToProcess(image.clone());
    img->Threshold(0.5);
    cv::Mat eroded;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
    cv::erode(img->GetImage(), eroded, element);
    img->SetStructuralElement(cross,{1,1});
    img->Erode();
    CheckImagesEqual(eroded, img->GetImage());
}

QTEST_APPLESS_MAIN(ProcessingImagesTest)

#include "tst_processingimagestest.moc"
