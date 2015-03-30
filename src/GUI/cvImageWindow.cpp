#include "cvImageWindow.h"
#include <QPainter>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>
#include <opencv2/opencv.hpp>
#include "../ApplicationManager.h"
#include <mutex>
std::mutex g_mutex;
namespace Mgr {

cvImageWindow::cvImageWindow(QString title, QObject *_parent)
  : slider(nullptr), parentObject(_parent) {
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
  setScene(&scene);
}

cvImageWindow::cvImageWindow(QGraphicsView *parent)
  : QGraphicsView(parent), scene(this) {
  closed = false;
}

void cvImageWindow::wheelEvent(QWheelEvent *event) {
  if (windowTitle().contains("Origin"))
    slider->setValue(slider->value() + event->delta() / 120);
}

void cvImageWindow::draw(cv::Mat img) {

  image.reset(new QImage(img.data, img.cols, img.rows, img.step,
                         QImage::Format_Indexed8));
  item.reset(new QGraphicsPixmapItem(QPixmap::fromImage(*image)));
  scene.clear();
  scene.addItem(item.get());
  show();
  if (windowTitle().contains("Origin"))
    slider->setFixedHeight(image->size().height());
  setFixedSize(image->size() + QSize(10, 10));
  rectangle = scene.addRect(0, 0, image->size().width(), image->size().height(),
                            QPen(Qt::white));
}

void cvImageWindow::setMaxValue(const int &value) { slider->setMaximum(value); }

void cvImageWindow::sliderValueChanged(const int &value) {
  emit(sliderValueChanged(value, this->windowTitle()));
}

void cvImageWindow::mousePressEvent(QMouseEvent *e) {
  mousePressed = true;
  rectPosition = e->pos();
}

void cvImageWindow::mouseMoveEvent(QMouseEvent *event) {
  if (!mousePressed)
    return;
  QPoint endPos = event->pos() - rectPosition;
  scene.removeItem(rectangle);
  rectangle = scene.addRect(rectPosition.rx(), rectPosition.ry(), endPos.rx(),
                            endPos.ry(), QPen(Qt::white));
}

void cvImageWindow::mouseReleaseEvent(QMouseEvent *releaseEvent) {
  mousePressed = false;
  rectEnd = releaseEvent->pos() - rectPosition;
  scene.removeItem(rectangle);
  rectangle = scene.addRect(rectPosition.rx(), rectPosition.ry(), rectEnd.rx(),
                            rectEnd.ry(), QPen(Qt::white));
}
}
