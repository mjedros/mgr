#pragma once

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include "Image3d.h"
#include <memory>
#include <mutex>
class VTKData {
  vtkSmartPointer<vtkRenderer> renderer;
  std::shared_ptr<Mgr::Image3d> image3d;

public:
  std::mutex rendererMutex;
  VTKData();
  void setImage3d(const std::shared_ptr<Mgr::Image3d> &image) {
    image3d = image;
  }
  void initVTKImage();
  void addNextImage(std::tuple<double, double, double> colors);
  vtkSmartPointer<vtkRenderer> getVTKRenderer() const { return renderer; }
  vtkSmartPointer<vtkActor>
  createActorOutOf3dImage(std::tuple<double, double, double> colors);
  std::vector<vtkSmartPointer<vtkActor>> actorsVector;

private:
  static void insertPoints(const int &threadNum,
                           const std::shared_ptr<Mgr::Image3d> &image3d,
                           vtkSmartPointer<vtkCellArray> vertices,
                           vtkSmartPointer<vtkPoints> points);
};
