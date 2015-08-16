#pragma once

#include "Image3d.h"
#include "ProcessingImage.h"
#include "ProcessingImage3d.h"
#include "OpenCLManager.h"
#include "csvFileUtils.h"
#include <QObject>
#include <QEvent>
#include <queue>

namespace Mgr {
class Image3d;
enum SourceType : u_int8_t;
enum class OPERATION : u_int8_t;
class Image3d;
enum class BackgroundSubstr : u_int8_t { NONE, FIRSTIMAGE, MOG2 };

/**
 * @brief Manadżer aplikacji.
 *
 * @details Klasa służy do zarządania procesem przetwarznia obrazu,
 * ustawiania parametrów i ich przechowywaniem.
 * Odpowiedzialna jest za obróbkę obrazu (odpowiednie wyciągnięcie ROI),
 * zarządza też sekwencjami operacji morfologicznych, ich zapisem i wczytywaniem
 * do pliku z rozszerzeniem *.csv.
 */
class ApplicationManager {
protected:
  OpenCLManager &openCLManager;
  std::string sourceFilename;
  Image3d image3d;
  std::shared_ptr<Image3d> processedImage3d;
  Image3d image3dPrevious;
  CsvFile csvFile;
  bool processROI = false;
  bool processOpenCv = false;
  BackgroundSubstr backgroundSubstr = BackgroundSubstr::NONE;
  ROI roi;

  /**
   * @brief Sprawdzenie czy podany ROI pasuje do wymiarów obrazu.
   */
  bool isROISizeValid(std::pair<int, int> imageSize);

  /**
   * @brief Metoda szablonowa wykonująca operację.
   * @details T jest sposobem wykonywania operacji - wzdłuż głębi, kolumn,
   * rzędów, lub trójwymiarowo.
   * I jest klasą służącą do przetwarzania obrazu w dwóch, lub trzech wymiarach.
   * @param operation Typ operacji
   * @param structuralElement Typ elementu strukturalnego
   * @param params Parametry elementu strukturalnego
   */
  template <class T, class I = ProcessingImage>
  void process(const OPERATION &operation, const std::string &structuralElement,
               const std::vector<float> &params);

public:
  ApplicationManager(OpenCLManager &openCLManagerRef)
    : openCLManager(openCLManagerRef) {}
  /**
   * @brief Inicjalizacja obrazu
   * @param source Typ źródła obrazu
   * @param name Nazwa filmu, obrazu, lub ścieżka folderu
   */
  void init(const SourceType &source, const std::string &name);

  /**
   * @brief Metoda inicjalizująca przetworzony obraz.
   * Wykonuje binaryzację obrazu wejściowego
   * @param minumum Wartość dolnego progu
   * @param maximum Wartość górnego progu
   */
  void initProcessedImage(const unsigned int &minumum = 100,
                          const unsigned int &maximum = 255);

  /**
   * @brief Metoda szablonowa ustawiająca ROI
   * @param processingImage Przetwarzany obraz
   */
  template <class I = ProcessingImage> void setROI(I &processingImage) {
    if (!processROI)
      return;
    processingImage.setROI(roi);
  }
  /**
   * @brief Metoda ustawiająca rozmiar ROI
   * @param roi ROI
   */
  void setROI(const ROI &newROI) { roi = newROI; }
  /**
   * @brief Zwraca rozmiar ROI
   */
  const ROI &getROI() const { return roi; }
  /**
   * @brief Metoda ustawiająca ma być przetwarzany ROI obrazu
   * @param processingImage Czy ma być przetwarzany ROI obrazu
   */
  void setProcessingROI(const bool processROINew) {
    processROI = processROINew;
  }
  /**
   * @brief Metoda ustawiająca czy obraz ma być przetwarzany za pomocą OpenCV
   * @param processingImage Czy ma być przetwarzany za pomocą OpenCV
   */
  void setProcessCv(const bool processCv) { processOpenCv = processCv; }

  /**
   * @brief Metoda ustawiająca czy źródłem obrazu jest obraz z kamery,
   * klatek filmu
   * @param imagesFromCam Czy kamera lub film jest źródłem obrazu
   */
  void setBackgroundExtraction(BackgroundSubstr background) {
    backgroundSubstr = background;
  }
  /**
   * @brief Metoda służąca do normalizacji obrazu wejściowego.
   */
  void normalizeOriginalImage();
  /**
   * @brief Metoda służąca do zapisania oryginalnego obrazu w postaci filmu.
   */
  void saveOriginalImage(const std::string &filename);
  /**
   * @brief @brief Metoda służąca do zapisania przetworzonego obrazu w postaci
   * filmu.
   */
  void saveProcessedImage(const std::string &filename);
  /**
   * @brief Metoda służąca do zapisu sekwencji do pliku *.csv
   */
  void saveCSVFile(const std::string &filename) { csvFile.saveFile(filename); }
  /**
   * @brief Metoda służąca do odczytu sekwencji z pliku *.csv
   */
  void loadCSVFile(const std::string &filename) { csvFile.loadFile(filename); }
  /**
   * @brief Metoda służąca do cofnięciu jeden operacji
   */
  void revertLastOperation() {
    processedImage3d.reset(new Image3d(image3dPrevious));
  }
  /**
   * @brief Metoda służąca do dodaniu operacji do sekwencji
   */
  void addToCSVFile(const std::vector<std::string> &operationsVector) {
    csvFile.addOperations(operationsVector);
  }
  /**
   * @brief Metoda usuwająca operację z sekwencji
   */
  void deleteOperation(const int &number) { csvFile.deleteOperation(number); }

  /**
   * @brief Metoda wykonująca przetwarzanie całej sekwencji
   */
  template <class T, class I = ProcessingImage> void processCsvSequence();

  void processCsvSequence(const std::string &operationWay);

  const OperationsVector &getOperationsVector() const {
    return csvFile.getOperationsVector();
  }
  /**
   * @brief Metoda zwracająca przetworzony obraz
   */
  const std::shared_ptr<Image3d> &getProcessedImage3d() const {
    return processedImage3d;
  }

  /**
   * @brief Metoda wykonująca przetwarzanie operacji
   * @param operationString Nazwa operacji
   * @param morphElementType Typ elementu strukturalnego
   * @param structElemParams Parametry elementu strukturalnego
   * @param operationWay Sposób wykonywanych operacji -wzdłuż głębi, kolumn,
   * rzędów
   */
  void process(const std::string &operationString,
               const std::string &morphElementType,
               const std::vector<float> structElemParams,
               const std::string &operationWay);
};
}
