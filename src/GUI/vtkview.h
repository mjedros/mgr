#pragma once

#include <vtkRenderWindow.h>
#include <QVTKInteractor.h>
#include <vtkSmartPointer.h>
#include "QVTKWidget.h"
#include <memory>
namespace Mgr {
class Image3d;
}
namespace Ui {
class VTKView;
}
class VTKData;
class VTKView : public QVTKWidget {
  Q_OBJECT

public:
  explicit VTKView(QObject *parentObj = nullptr);
  void setImage3d(const std::shared_ptr<Mgr::Image3d> &image);
  void initImage();
  void renderNewImage(std::tuple<double, double, double> colors);
  void render();
  ~VTKView();

protected:
  virtual void showEvent(QShowEvent *);
  virtual void mousePressEvent(QMouseEvent *);
  virtual void mouseReleaseEvent(QMouseEvent *);

private:
  std::shared_ptr<Mgr::Image3d> image3d;
  std::unique_ptr<Ui::VTKView> ui;
  std::unique_ptr<VTKData> vtkData;
  vtkSmartPointer<QVTKInteractor> iren;
  vtkSmartPointer<vtkRenderWindow> renWin;
  QObject *parentObject;
signals:
  void showVtkImage();
};
