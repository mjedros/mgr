#include "GUI/cvImageWindow.h"

#include "ApplicationManager.h"
#include <QPainter>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>
#include <opencv2/opencv.hpp>

namespace Mgr {

cvImageWindow::cvImageWindow(QString title, QObject *_parent)
  : item(nullptr), rectangle(0), slider(nullptr), parentObject(_parent) {
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

  } else if (title.contains("Processed"))
    connect(this, SIGNAL(setRectangle(QPoint, QPoint)), parentObject,
            SLOT(setRectangle(QPoint, QPoint)));
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
  if (image != nullptr)
    scene.removeItem(item.get());
  image.reset(new QImage(img.data, img.cols, img.rows, img.step,
                         QImage::Format_Indexed8));
  item.reset(new QGraphicsPixmapItem(QPixmap::fromImage(*image)));
  scene.addItem(item.get());
  show();
  if (windowTitle().contains("Origin"))
    slider->setFixedHeight(image->size().height());
  setFixedSize(image->size() + QSize(10, 10));
}

void cvImageWindow::setMaxValue(const int &value) { slider->setMaximum(value); }

void cvImageWindow::sliderValueChanged(const int &value) {
  emit(sliderValueChanged(value, this->windowTitle()));
}

void cvImageWindow::mousePressEvent(QMouseEvent *e) {
  mousePressed = true;
  firstPosition = e->pos();
}

void cvImageWindow::mouseMoveEvent(QMouseEvent *event) {
  QPoint currentPos = event->pos();
  if (!mousePressed)
    return;

  if (rectangle) {
    scene.removeItem(rectangle);
    delete rectangle;
  }
  rectSize = QPoint(std::abs(firstPosition.rx() - currentPos.rx()),
                    std::abs(firstPosition.ry() - currentPos.ry()));
  rectPosition = QPoint(std::min(firstPosition.rx(), currentPos.rx()),
                        std::min(firstPosition.ry(), currentPos.ry()));

  rectangle = scene.addRect(rectPosition.rx(), rectPosition.ry(), rectSize.rx(),
                            rectSize.ry(), QPen(Qt::white));
}

void cvImageWindow::mouseReleaseEvent(QMouseEvent *releaseEvent) {
  mousePressed = false;
  QPoint currentPos = releaseEvent->pos();
  if (currentPos == firstPosition)
    return;
  rectangle->setZValue(999);

  emit(setRectangle(rectPosition, rectPosition + rectSize));
}
}
