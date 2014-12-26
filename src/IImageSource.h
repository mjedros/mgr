#pragma once

namespace cv {
class Mat;
class VideoCapture;
}
/**
 * @brief Image source interface.
 *
 * @details Abstract class with methods derived by different sources of images.
 *
 */
class IImageSource {
  public:
    /**
     * @brief Start image aquisition.
     */
    virtual void Start() = 0;
    /**
     * @brief Stop image aquisition.
     */
    virtual void Stop() = 0;
    /**
     * @brief Get next image in queue.
     */
    virtual cv::Mat Get() = 0;
};
