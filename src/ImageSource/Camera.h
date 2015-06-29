#pragma once
#include "ImageSource/IImageSource.h"

#include <memory>
#include <set>
namespace Mgr {
class Camera : public IImageSource {
  std::unique_ptr<cv::VideoCapture> videoCapture;

public:
  /**
   * @brief Constructor
   */
  Camera();
  virtual ~Camera();
  virtual void Start() override;
  virtual void Stop() override;
  cv::Mat Get() override;
};
}
