#pragma once
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QImage>
#include <QKeyEvent>

#include <opencv/highgui.h>

class cvImageWindow : public QMainWindow
{
   Q_OBJECT

 public:
   bool closed;
   cvImageWindow();
   ~cvImageWindow();
   void paintEvent(QPaintEvent *e);
   void closeEvent(QCloseEvent *e);
   void draw(cv::Mat image);

 private:
   QImage *image;
};
