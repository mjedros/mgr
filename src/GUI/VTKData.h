#pragma once

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include "Image3d.h"
#include <memory>
#include <mutex>
class vtkActor;
class vtkPolyDataMapper;
class vtkPoints;
class vtkPolyData;
class VTKData {
  vtkSmartPointer<vtkRenderer> renderer;
  std::shared_ptr<Mgr::Image3d> image3d;
  struct dataMem {
  public:
    vtkCellArray *vertices;
    vtkPolyDataMapper *mapper;
    vtkPoints *points;
    ~dataMem();
  };
  std::vector<dataMem> dataMemHolder;

public:
  std::mutex rendererMutex;
  VTKData();
  ~VTKData();
  void setImage3d(const std::shared_ptr<Mgr::Image3d> &image) {
    image3d.reset();
    image3d = image;
  }
  void initVTKImage();
  void addNextImage(std::tuple<double, double, double> colors);
  vtkSmartPointer<vtkRenderer> getVTKRenderer() const { return renderer; }
  vtkActor *createActorOutOf3dImage(std::tuple<double, double, double> colors);

private:
  static void insertPoints(const int &threadNum,
                           const std::shared_ptr<Mgr::Image3d> &image3d,
                           vtkCellArray *vertices, vtkPoints *points);
};
