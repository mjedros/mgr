#include "cvImageWindow.h"
#include <QLabel>
#include <QPainter>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>

#include <opencv/cv.h>

cvImageWindow::cvImageWindow() : image(NULL)
{
   closed = false;
   setWindowTitle(tr("Processed image"));
}

cvImageWindow::~cvImageWindow()
{
   if (image)
      delete image;
}

void cvImageWindow::paintEvent(QPaintEvent *e)
{
   QPainter painter(this);
   if (!image)
      return;
   painter.drawImage(QPoint(0, 0), *image);
   QWidget::paintEvent(e);
}

void cvImageWindow::closeEvent(QCloseEvent *e)
{
   closed = true;
   QWidget::closeEvent(e);
}
void cvImageWindow::draw(cv::Mat img)
{
   if (image)
   {
      delete image;
   }
   resize(img.size().width, img.size().height);
   cv::cvtColor(img, img, CV_GRAY2RGB);
   image = new QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
   update();
}
