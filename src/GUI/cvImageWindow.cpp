#include "cvImageWindow.h"
#include <QPainter>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>
#include <opencv2/opencv.hpp>
#include "../ApplicationManager.h"
namespace Mgr {

cvImageWindow::cvImageWindow(QString title, QObject *_parent)
  : imgDisplayLabel(this), slider(nullptr), parentObject(_parent) {
  closed = false;
  setWindowTitle(title);
  this->setMouseTracking(true);
  if (title.contains("Origin")) {
    slider.reset(new QSlider(this));
    slider->setValue(0);
    connect(slider.get(), SIGNAL(valueChanged(int)), this,
            SLOT(sliderValueChanged(int)));
    connect(this, SIGNAL(sliderValueChanged(const int &, const QString)),
            parentObject,
            SLOT(sliderValueChanged(const int &, const QString &)));
  }
}

cvImageWindow::cvImageWindow(QGraphicsView *parent)
  : QGraphicsView(parent), imgDisplayLabel(this), scene(this) {
  closed = false;
  setScene(&scene);
  this->show();
}

void cvImageWindow::wheelEvent(QWheelEvent *event) {
  if (windowTitle().contains("Origin"))
    slider->setValue(slider->value() + event->delta() / 120);
}

void cvImageWindow::draw(cv::Mat img) {

  image.reset(new QImage(img.data, img.cols, img.rows, img.step,
                         QImage::Format_Indexed8));
  item.reset(new QGraphicsPixmapItem(QPixmap::fromImage(*image)));
  scene.addItem(item.get());
  this->setScene(&scene);
  this->show();
  if (windowTitle().contains("Origin"))
    slider->setFixedHeight(image->size().height());
  this->setFixedSize(image->size() + QSize(10, 10));
}

void cvImageWindow::setMaxValue(const int &value) { slider->setMaximum(value); }

void cvImageWindow::sliderValueChanged(const int &value) {
  emit(sliderValueChanged(value, this->windowTitle()));
}

void cvImageWindow::mousePressEvent(QMouseEvent *e) {
  std::cout << "pressed " << e->globalX() << ", " << e->globalY() << std::endl;
}

void cvImageWindow::mouseMoveEvent(QMouseEvent *) {}

void cvImageWindow::mouseReleaseEvent(QMouseEvent *releaseEvent) {
  std::cout << "released " << releaseEvent->globalX() << ", "
            << releaseEvent->globalY() << std::endl;
}
}
