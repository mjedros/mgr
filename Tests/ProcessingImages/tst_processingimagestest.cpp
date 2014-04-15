#include <QString>
#include <QtTest>
#include "../../src/ProcessingImage.h"
#include "../../src/OpenCLManager.h"
class ProcessingImagesTest : public QObject
{
   Q_OBJECT
   std::shared_ptr<OpenCLManager> openCLManager;

 public:
   ProcessingImagesTest();
 private Q_SLOTS:
   void EmptyImageTest();
   void SimpleImageTest();
   void BinarizeTest();
};

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
   cv::Mat ones = cv::Mat::ones(10, 10, 1);
   img->SetImageToProcess(ones.clone());
   img->Threshold(1);
   cv::Mat zeros = cv::Mat::zeros(ones.cols, ones.rows, 1);
   QCOMPARE(*zeros.data, *(img->GetImage()).data);
}

QTEST_APPLESS_MAIN(ProcessingImagesTest)

#include "tst_processingimagestest.moc"
