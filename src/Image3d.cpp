#include "Image3d.h"

void Image3d::setImageAtDepth(const int &_depth, const cv::Mat image2d) {
    image2d.copyTo(Image.colRange(cols * _depth, cols * (_depth + 1)));
}

void Image3d::setImageAtRow(const int &row, const cv::Mat image2d) {
    for (int j = 0; j < depth; j++) {
        (image2d.row(j))
            .copyTo(Image.row(row).colRange(j * cols, (j + 1) * cols));
    }
}

void Image3d::setImageAtCol(const int &col, const cv::Mat image2d) {
    for (int j = 0; j < depth; j++) {
        (image2d.col(j)).copyTo(Image.col(col + cols * j));
    }
}

const cv::Mat Image3d::getImageAtDepth(const int &_depth) const {
    return Image.colRange(cols * _depth, cols * (_depth + 1));
}

const cv::Mat Image3d::getImageAtRow(const int &row) const {
    cv::Mat image2d(depth, cols, Image.type());
    for (int j = 0; j < depth; j++) {
        (Image.row(row).colRange(cols * j, cols * (j + 1)))
            .copyTo(image2d.row(j));
    }
    return image2d;
}

const cv::Mat Image3d::getImageAtCol(const int &col) const {
    cv::Mat image2d(rows, depth, Image.type());
    for (int j = 0; j < depth; j++) {
        (Image.col(col + cols * j)).copyTo(image2d.col(j));
    }
    return image2d;
}

Image3d::Image3d(const int &_depth, const cv::Mat image2d) : depth(_depth) {
    rows = image2d.rows;
    cols = image2d.cols;
    Image = cv::Mat(rows, cols * _depth, image2d.type());
}
