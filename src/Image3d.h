#pragma once

#include <opencv2/opencv.hpp>
namespace Mgr {
class Image3d {
private:
  cv::Mat image;
  int depth;
  int rows;
  int cols;

public:
  inline const int &getDepth() const { return depth; }
  inline const int &getRows() const { return rows; }
  inline const int &getCols() const { return cols; }
  void setImageAtDepth(const int &depth, const cv::Mat image2d);
  void setImageAtRow(const int &row, const cv::Mat image2d);
  void setImageAtCol(const int &col, const cv::Mat image2d);
  void set3dImage(cv::Mat newImage) { image = newImage; }
  const cv::Mat getImageAtDepth(const int &_depth) const;
  const cv::Mat getImageAtRow(const int &row) const;
  const cv::Mat getImageAtCol(const int &col) const;
  const cv::Mat get3dMatImage() const { return image; }
  Image3d(const int &_depth, const cv::Mat &image2d);
  Image3d(const Image3d &source);
  Image3d() {}
  ~Image3d() { image.release(); }
  Image3d &operator=(const Image3d &other);
};
}
