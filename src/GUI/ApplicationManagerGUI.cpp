#include "ApplicationManagerGUI.h"
#include "../src/Image3d.h"
namespace Mgr {
void ApplicationManagerGUI::showImages() {
  originalWindow.reset(new cvImageWindow("Depth Original", this));
  processedWindow.reset(new cvImageWindow("Depth Processed", this));
  colsOriginal.reset(new cvImageWindow("Cols Original", this));
  colsProcessed.reset(new cvImageWindow("Cols Processed", this));
  setMaxValues();
  showWindows(image3d->getDepth() / 2);
  showCols(image3d->getCols() / 2);
}

void ApplicationManagerGUI::sliderValueChanged(const int &value,
                                               const QString &title) {
  if (title.startsWith("Depth"))
    showWindows(value);
  else if (title.startsWith("Cols"))
    showCols(value);
}

void ApplicationManagerGUI::showWindows(const int &depth) {
  originalWindow->draw(image3d->getImageAtDepth(depth));
  processedWindow->draw(processedImage3d->getImageAtDepth(depth));
  processedWindow->update();
  originalWindow->update();
}

void ApplicationManagerGUI::closeWindows() {
  colsProcessed->close();
  colsOriginal->close();
  processedWindow->close();
  originalWindow->close();
}

void ApplicationManagerGUI::showCols(const int &col) {
  colsOriginal->draw(image3d->getImageAtCol(col));
  colsProcessed->draw(processedImage3d->getImageAtCol(col));
  colsProcessed->update();
  colsOriginal->update();
}

void ApplicationManagerGUI::setMaxValues() {
  originalWindow->setMaxValue(image3d->getDepth() - 1);
  colsOriginal->setMaxValue(image3d->getCols() - 1);
}
}
