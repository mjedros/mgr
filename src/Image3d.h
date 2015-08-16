#pragma once

#include <opencv2/opencv.hpp>
namespace Mgr {
/**
 * @brief Klasa reprezentująca obraz 3D
 * @details Obraz przechowywany jest w formacie cv::Mat,
 * klasa udostępnia interfejs umożliwiający pobranie obrazu 2D z odpowiedniej
 * kolumny, rzędu lub głębi.
 */
class Image3d {
private:
  cv::Mat image;
  int depth = 0;
  int rows = 0;
  int cols = 0;

public:
  /**
   * @brief Metoda zwracająca wielkość głębi obrazu.
   */
  inline const int &getDepth() const { return depth; }
  /**
   * @brief Metoda zwracająca rozmiar rzędów.
   */
  inline const int &getRows() const { return rows; }
  /**
   * @brief Metoda zwracająca rozmiar kolumn.
   */
  inline const int &getCols() const { return cols; }
  /**
   * @brief Metoda ustawiająca obraz 2D w odpowiedniej głębi.
   * @details Rozmiar obrazu musi się zgadzać z rozmiarem obrazu w głębi
   * @param depth Głębia w której ma zostać ustawiony obraz
   * @param image2d Obraz 2D
   */
  void setImageAtDepth(const int &depth, const cv::Mat &image2d);
  /**
   * @brief Metoda ustawiająca obraz 2D w odpowiednim rzędzie.
   * @details Rozmiar obrazu musi się zgadzać z rozmiarem obrazu w rzędzie
   * @param row Rząd w którym ma zostać ustawiony obraz
   * @param image2d Obraz 2D
   */
  void setImageAtRow(const int &row, const cv::Mat image2d);
  /**
   * @brief Metoda ustawiająca obraz 2D w odpowiedniej kolumnie.
   * @details Rozmiar obrazu musi się zgadzać z rozmiarem obrazu w kolumnie
   * @param col Kolumna w której ma zostać ustawiony obraz
   * @param image2d Obraz 2D
   */
  void setImageAtCol(const int &col, const cv::Mat image2d);
  /**
   * @brief Metoda ustawiająca obraz 3D
   */
  void set3dImage(cv::Mat newImage) { image = newImage; }
  /**
   * @brief Metoda zwracająca obraz 2D z odpowiedniej głębi
   */
  const cv::Mat getImageAtDepth(const int &_depth) const;
  /**
   * @brief Metoda zwracająca obraz 2D z odpowiedniego rzędu
   */
  const cv::Mat getImageAtRow(const int &row) const;
  /**
   * @brief Metoda zwracająca obraz 2D z odpowiedniej kolumny
   */
  const cv::Mat getImageAtCol(const int &col) const;
  /**
   * @brief Metoda zwracająca obraz 3D.
   */
  inline uchar &getPoint(const int col, const int row, const int depth) {
    return image.at<uchar>(depth, row * cols + col);
  }
  const cv::Mat get3dMatImage() const { return image; }
  bool isImage3dInitialized() { return depth != 0; }
  Image3d(const int &_depth, const cv::Mat &image2d);
  Image3d(const Image3d &source);
  Image3d(const int newRows, const int newCols, const cv::Mat &image3d);
  Image3d() {}
  void clear() { image.release(); }
  ~Image3d() { image.release(); }
  Image3d &operator=(const Image3d &other);
  inline void setPoint(const int col, const int row, const int depth,
                       const unsigned char value) {
    image.at<uchar>(depth, row * cols + col) = value;
  }
};
}
