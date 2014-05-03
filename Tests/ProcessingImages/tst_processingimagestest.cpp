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
   void DilateCrossTest();
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
    cv::Mat result;
    img->Dilate();
    cv::compare(dilated,img->GetImage(),result,cv::CMP_EQ);
    cv::imwrite("../../Data/wynikOpencv.jpg",dilated);
    cv::imwrite("../../Data/wynik.jpg",img->GetImage());
    cv::imwrite("../../Data/minus.jpg",dilated-img->GetImage());
    int nz = cv::countNonZero(result);
    std::cout<<image.cols*image.rows<< "  -  "<<nz << " = "<< image.cols*image.rows- nz <<std::endl;
    QVERIFY(nz == image.cols*image.rows);
}

QTEST_APPLESS_MAIN(ProcessingImagesTest)

#include "tst_processingimagestest.moc"
