#pragma once

namespace cv {
class Mat;
class VideoCapture;
}
namespace Mgr {
/**
 * @brief Interfejs źródła obrazu
 *
 * @details Klasa abstrakcyjna dziedziczona przez typy źródeł obrazu.
 * Wszystkie źródła obrazu - kamera, folder i film są tworzone przez
 * fabrykę abstrakcyjną.
 */
class IImageSource {
public:
  /**
   * @brief Start akwizycji obrazu
   */
  virtual void Start() = 0;
  /**
   * @brief Koniec akwizycji obrazu
   */
  virtual void Stop() = 0;
  /**
   * @brief Metoda zwracająca następny obraz ze źródła
   */
  virtual cv::Mat Get() = 0;
};
}
