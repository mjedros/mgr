#ifndef VTKDATA_H
#define VTKDATA_H
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include "../src/Image3d.h"
#include <memory>
class VTKData {
  vtkSmartPointer<vtkRenderer> renderer;
  std::shared_ptr<Mgr::Image3d> image3d;

public:
  VTKData();
  void setImage3d(const std::shared_ptr<Mgr::Image3d> &image) {
    image3d = image;
  }
  void initVTKImage();
  void addNextImage(std::tuple<double, double, double> colors);
  vtkSmartPointer<vtkRenderer> getVTKRenderer() const { return renderer; }
  vtkSmartPointer<vtkActor>
  createActorOutOf3dImage(std::tuple<double, double, double> colors);
};

#endif // VTKDATA_H
