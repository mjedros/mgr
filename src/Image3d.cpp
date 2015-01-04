#include "Image3d.h"
using namespace Mgr;

void Image3d::setImageAtDepth(const int &_depth, const cv::Mat image2d) {
    image2d.copyTo(image.colRange(cols * _depth, cols * (_depth + 1)));
}

void Image3d::setImageAtRow(const int &row, const cv::Mat image2d) {
    for (int j = 0; j < depth; j++) {
        (image2d.row(j))
            .copyTo(image.row(row).colRange(j * cols, (j + 1) * cols));
    }
}

void Image3d::setImageAtCol(const int &col, const cv::Mat image2d) {
    for (int j = 0; j < depth; j++) {
        (image2d.col(j)).copyTo(image.col(col + cols * j));
    }
}

const cv::Mat Image3d::getImageAtDepth(const int &_depth) const {
    return image.colRange(cols * _depth, cols * (_depth + 1));
}

const cv::Mat Image3d::getImageAtRow(const int &row) const {
    cv::Mat image2d(depth, cols, image.type());
    for (int j = 0; j < depth; j++) {
        (image.row(row).colRange(cols * j, cols * (j + 1)))
            .copyTo(image2d.row(j));
    }
    return image2d;
}

const cv::Mat Image3d::getImageAtCol(const int &col) const {
    cv::Mat image2d(rows, depth, image.type());
    for (int j = 0; j < depth; j++) {
        (image.col(col + cols * j)).copyTo(image2d.col(j));
    }
    return image2d;
}

Image3d::Image3d(const int &_depth, const cv::Mat image2d) : depth(_depth) {
    rows = image2d.rows;
    cols = image2d.cols;
    image = cv::Mat(rows, cols * _depth, image2d.type());
}

Image3d::Image3d(const Image3d &source)
    : depth(source.depth), rows(source.rows), cols(source.cols) {
    image = source.image.clone();
}
