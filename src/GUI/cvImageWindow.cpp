#include "cvImageWindow.h"
#include <QLabel>
#include <QPainter>
#include <QFileDialog>
#include <QApplication>
#include <QStatusBar>
#include <opencv2/opencv.hpp>
#include "../ApplicationManager.h"
using namespace Mgr;

cvImageWindow::cvImageWindow(QString title, QObject *_parent)
    : imgDisplayLabel(this), slider(nullptr), parentObject(_parent) {
    closed = false;
    setWindowTitle(title);
    if (title.contains("Origin")) {
        slider.reset(new QSlider(this));
        connect(slider.get(), SIGNAL(valueChanged(int)), this,
                SLOT(sliderValueChanged(int)));
        connect(this, SIGNAL(sliderValueChanged(const int &, const QString)),
                parentObject,
                SLOT(sliderValueChanged(const int &, const QString &)));
    }
}

cvImageWindow::cvImageWindow(QDialog *parent)
    : QDialog(parent), imgDisplayLabel(this) {
    closed = false;
    this->show();
}

void cvImageWindow::wheelEvent(QWheelEvent *event) {
    if (windowTitle().contains("Origin"))
        slider->setValue(slider->value() + event->delta() / 120);
}

void cvImageWindow::draw(cv::Mat img) {
    this->show();
    image.reset(new QImage(img.data, img.cols, img.rows, img.step,
                           QImage::Format_Indexed8));
    imgDisplayLabel.setPixmap(QPixmap::fromImage(*image));
    imgDisplayLabel.adjustSize();
    if (windowTitle().contains("Origin"))
        slider->setFixedHeight(imgDisplayLabel.size().height());
    this->adjustSize();
}

void cvImageWindow::setMaxValue(const int &value) { slider->setMaximum(value); }

void cvImageWindow::sliderValueChanged(const int &value) {
    emit(sliderValueChanged(value, this->windowTitle()));
}
