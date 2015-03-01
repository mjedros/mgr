#pragma once
#include <QDialog>
#include <QMouseEvent>
#include <QImage>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QLabel>
#include <memory>
#include <QSlider>

#include <opencv/highgui.h>
namespace Mgr {
class cvImageWindow : public QDialog {
    Q_OBJECT

  public:
    bool closed;
    cvImageWindow(QString title = "", QObject *_parent = 0);
    explicit cvImageWindow(QDialog *parent = 0);
    void wheelEvent(QWheelEvent *event);
    void draw(cv::Mat image);
    void setMaxValue(const int &value);
  private slots:
    void sliderValueChanged(const int &value);

  private:
    std::unique_ptr<QImage> image;
    QLabel imgDisplayLabel;

    std::unique_ptr<QSlider> slider;
    QObject *parentObject;
signals:
    void sliderValueChanged(const int &value, const QString &title);
};
}
