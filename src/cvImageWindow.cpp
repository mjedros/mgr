#include "cvImageWindow.h"
#include <QLabel>
#include <QPainter>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>

#include <opencv/cv.h>

cvImageWindow::cvImageWindow() : _image(NULL) { setWindowTitle(tr("Processed image")); }

cvImageWindow::~cvImageWindow()
{
   if (_image)
      delete _image;
}

void cvImageWindow::paintEvent(QPaintEvent *e)
{
   QPainter painter(this);
   if (!_image)
      return;
   painter.drawImage(QPoint(0, 0), *_image);
   QWidget::paintEvent(e);
}

void cvImageWindow::closeEvent(QCloseEvent *e)
{
   closed = true;
   QWidget::closeEvent(e);
}
void cvImageWindow::draw(cv::Mat img)
{
   if (_image)
   {
      delete _image;
   }
   else
   {
      resize(img.size().width, img.size().height);
   }
   cv::cvtColor(img, img, CV_GRAY2RGB);
   _image = new QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
   update();
}
