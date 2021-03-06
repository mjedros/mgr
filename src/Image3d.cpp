#include "Image3d.h"

namespace Mgr {
void Image3d::setImageAtDepth(const int &_depth, const cv::Mat &image2d) {
  size_t sizeOfImg = image2d.rows * image2d.cols * sizeof(uchar);
  memmove(image.data + _depth * sizeOfImg, image2d.data, sizeOfImg);
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
  cv::Mat image2d(rows, cols, image.type());
  const size_t sizeOfImg = image2d.rows * image2d.cols * sizeof(uchar);
  memmove(image2d.data, image.data + _depth * sizeOfImg, sizeOfImg);
  return image2d;
}

const cv::Mat Image3d::getImageAtRow(const int &row) const {
  return image.colRange(row * cols, (row + 1) * cols);
}

const cv::Mat Image3d::getImageAtCol(const int &col) const {

  cv::Mat image2d(rows, depth, image.type());
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < depth; ++j) {
      image2d.at<uchar>(i, j) = image.at<uchar>(j, i * cols + col);
    }
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

Image3d::Image3d(const int newRows, const int newCols, const cv::Mat &image3d) {
  rows = newRows;
  cols = newCols;
  depth = image3d.rows;
  image = image3d;
}
}
