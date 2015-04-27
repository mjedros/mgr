#include "Normalization.h"

namespace Mgr {
static const int NORMALIZATION_WIDTH = 8;
bool checkBrighness(cv::Mat &image, const int &level) {
  int sum = 0;
  for (int col = 0; col < image.cols; col++) {
    for (int row = 0; row < image.rows; row++) {
      sum += image.at<uchar>(row, col);
    }
  }
  return ((sum / (image.cols * image.rows) - level) < 2);
}
void changeContrast(cv::Mat &image, const double &alfa) {
  for (int col = 0; col < image.cols; col++) {
    for (int row = 0; row < image.rows; row++) {
      if (alfa > 1) {
        if ((image.at<uchar>(row, col) * alfa) > 255) {
          image.at<uchar>(row, col) = 255;
        }
      } else
        image.at<uchar>(row, col) *= alfa;
    }
  }
}

void changeBrightness(cv::Mat &image, const int &level) {
  for (int col = 0; col < image.cols; col++) {
    for (int row = 0; row < image.rows; row++) {
      if ((image.at<uchar>(row, col) - level > 0)) {
        image.at<uchar>(row, col) -= level;
      }
    }
  }
}
int setImageLight(Image3d &image3d, const unsigned int &depth,
                  const int &newLevel) {
  cv::Mat image = image3d.getImageAtDepth(depth);
  while (!checkBrighness(image, newLevel)) {
    changeContrast(image, 0.9);
    changeBrightness(image, 2);
  }
  image3d.setImageAtDepth(depth, image);
  return newLevel;
}

void normalize(Image3d &image3d) {
  for (auto i = 0; i <= image3d.getDepth() - NORMALIZATION_WIDTH; i++) {
    std::vector<int> levels(NORMALIZATION_WIDTH, 0);
    for (int curId = 0; curId < NORMALIZATION_WIDTH; curId++) {
      cv::Mat image = image3d.getImageAtDepth(i + curId).clone();
      for (int col = 0; col < image.cols; col++) {
        for (int row = 0; row < image.rows; row++) {
          levels[curId] += image.at<uchar>(row, col);
        }
      }
      levels[curId] = levels[curId] / (image.cols * image.rows);
    }
    const int avarage =
        std::accumulate(levels.begin(), levels.end(), 0) / NORMALIZATION_WIDTH;
    for (unsigned int curId = 1; curId < NORMALIZATION_WIDTH; curId++) {
      if (levels[curId] - avarage > 5) {
        levels[curId] = setImageLight(image3d, i + curId, levels[curId - 1]);
      }
    }
  }
}
}
