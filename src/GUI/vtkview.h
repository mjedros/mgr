#ifndef VTKVIEW_H
#define VTKVIEW_H
#include <vtkRenderWindow.h>
#include <QVTKInteractor.h>
#include <vtkSmartPointer.h>
#include <QWidget>
#include <memory>
namespace Mgr {
class Image3d;
}
namespace Ui {
class VTKView;
}
class VTKData;
class VTKView : public QWidget {
  Q_OBJECT

public:
  explicit VTKView(std::unique_ptr<QWidget> parent = nullptr);
  void setImage3d(const std::shared_ptr<Mgr::Image3d> &image);
  void initImage();
  void renderNewImage(std::tuple<double, double, double> colors);
  ~VTKView();

private:
  std::shared_ptr<Mgr::Image3d> image3d;
  std::unique_ptr<Ui::VTKView> ui;
  std::unique_ptr<VTKData> vtkData;
  vtkSmartPointer<QVTKInteractor> iren;
  vtkSmartPointer<vtkRenderWindow> renWin;
};

#endif // VTKVIEW_H
