#include "Image3d.h"

namespace Mgr {
void Image3d::setImageAtDepth(const int &_depth, const cv::Mat image2d) {
  for (int i = 0; i < rows; ++i)
    image2d.row(i).copyTo(image.row(_depth).colRange(i * cols, (i + 1) * cols));
  // size_t sizeOfImg = image2d.rows * image2d.cols * sizeof(uchar);
  // memmove(image.data + _depth * sizeOfImg, image2d.data, sizeOfImg);
}

void Image3d::setImageAtRow(const int &row, const cv::Mat image2d) {
  image2d.copyTo(image.colRange(row * cols, (row + 1) * cols));
}

void Image3d::setImageAtCol(const int &col, const cv::Mat image2d) {
  for (int j = 0; j < rows; j++) {
    cv::Mat img = image2d.row(j);
    cv::transpose(img, img);
    img.copyTo(image.col(col + j * rows));
  }
}

const cv::Mat Image3d::getImageAtDepth(const int &_depth) const {
  //  cv::Mat image2d(rows, cols, image.type());
  //  const size_t sizeOfImg = image2d.rows * image2d.cols * sizeof(uchar);
  //  memmove(image2d.data, image.data + _depth * sizeOfImg, sizeOfImg);
  //  return image2d;
  cv::Mat image2d(rows, cols, image.type());
  for (int j = 0; j < rows; j++) {
    (image.row(_depth).colRange(cols * j, cols * (j + 1)))
        .copyTo(image2d.row(j));
  }
  return image2d;
}

const cv::Mat Image3d::getImageAtRow(const int &row) const {
  return image.colRange(row * cols, (row + 1) * cols);
}

const cv::Mat Image3d::getImageAtCol(const int &col) const {
  cv::Mat image2d(rows, depth, image.type());
  for (int i = 0; i < rows; ++i) {
    cv::Mat colImg = image.col(col + i * rows);
    cv::transpose(colImg, colImg);
    colImg.copyTo(image2d.row(i));
  }
  return image2d;
}

Image3d &Image3d::operator=(const Image3d &other) {
  if (this != &other) {
    image = other.image.clone();
    depth = other.depth;
    rows = other.rows;
    cols = other.cols;
  }
  return *this;
}

Image3d::Image3d(const int &_depth, const cv::Mat &image2d) : depth(_depth) {
  rows = image2d.rows;
  cols = image2d.cols;
  image = cv::Mat(_depth, rows * cols, image2d.type());
}

Image3d::Image3d(const Image3d &source)
  : depth(source.depth), rows(source.rows), cols(source.cols) {
  image = source.image.clone();
}
}
