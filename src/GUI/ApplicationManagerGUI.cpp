#include "ApplicationManagerGUI.h"

namespace Mgr {
void ApplicationManagerGUI::showImages() {
  originalWindow.reset(new cvImageWindow("Depth Original", this));
  processedWindow.reset(new cvImageWindow("Depth Processed", this));
  rowsOriginal.reset(new cvImageWindow("Rows Original", this));
  rowsProcessed.reset(new cvImageWindow("Rows Processed", this));
  colsOriginal.reset(new cvImageWindow("Cols Original", this));
  colsProcessed.reset(new cvImageWindow("Cols Processed", this));
  setMaxValues();

  showWindows(image3d.getDepth() / 2);
  showCols(image3d.getCols() / 2);
  showRows(image3d.getRows() / 2);
}

void ApplicationManagerGUI::sliderValueChanged(const int &value,
                                               const QString &title) {
  if (title.startsWith("Depth"))
    showWindows(value);
  else if (title.startsWith("Cols"))
    showCols(value);
  else
    showRows(value);
}

void ApplicationManagerGUI::setRectangle(QPoint startPoint, QPoint endPoint) {
  roi = std::make_pair(std::make_pair(startPoint.rx(), endPoint.rx()),
                       std::make_pair(startPoint.ry(), endPoint.ry()));
  processROI = true;
}

void ApplicationManagerGUI::showWindows(const int &depth) {
  originalWindow->draw(image3d.getImageAtDepth(depth));
  processedWindow->draw(processedImage3d->getImageAtDepth(depth));
  processedWindow->update();
  originalWindow->update();
}

void ApplicationManagerGUI::closeWindows() {
  if (originalWindow == nullptr)
    return;
  colsProcessed->close();
  colsOriginal->close();
  rowsProcessed->close();
  rowsOriginal->close();
  processedWindow->close();
  originalWindow->close();
}
void ApplicationManagerGUI::showRows(const int &row) {
  rowsOriginal->draw(image3d.getImageAtRow(row));
  rowsProcessed->draw(processedImage3d->getImageAtRow(row));
  rowsProcessed->update();
  rowsOriginal->update();
}
void ApplicationManagerGUI::showCols(const int &col) {
  colsOriginal->draw(image3d.getImageAtCol(col));
  colsProcessed->draw(processedImage3d->getImageAtCol(col));
  colsProcessed->update();
  colsOriginal->update();
}

void ApplicationManagerGUI::setMaxValues() {
  originalWindow->setMaxValue(image3d.getDepth() - 1);
  colsOriginal->setMaxValue(image3d.getCols() - 1);
  rowsOriginal->setMaxValue(image3d.getRows() - 1);
}
}
