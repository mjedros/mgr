#pragma once
#include <QGraphicsView>
#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QImage>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QLabel>
#include <memory>
#include <QSlider>
#include <QGraphicsPixmapItem>
#include <opencv/highgui.h>
namespace Mgr {
class cvImageWindow : public QGraphicsView {
  Q_OBJECT

public:
  bool closed;
  cvImageWindow(QString title = "", QObject *_parent = 0);
  explicit cvImageWindow(QGraphicsView *parent = 0);
  void wheelEvent(QWheelEvent *event);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *releaseEvent);
  void draw(cv::Mat image);
  void setMaxValue(const int &value);

private slots:
  void sliderValueChanged(const int &value);

private:
  bool mousePressed;
  std::unique_ptr<QImage> image;
  QGraphicsScene scene;
  std::unique_ptr<QGraphicsPixmapItem> item;
  QGraphicsRectItem *rectangle;
  QPoint rectPosition;
  QPoint firstPosition;
  QPoint rectSize;

  std::unique_ptr<QSlider> slider;
  QObject *parentObject;
signals:
  void sliderValueChanged(const int &value, const QString &title);
  void setRectangle(QPoint startPoint, QPoint endPoint);
};
}
