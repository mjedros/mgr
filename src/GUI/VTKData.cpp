#include "GUI/VTKData.h"

#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkContourFilter.h>
#include <vtkCellArray.h>
VTKData::VTKData() : renderer(vtkRenderer::New()) {
  renderer->SetBackground(.1, .1, .1);
}

vtkSmartPointer<vtkActor>
VTKData::createActorOutOf3dImage(std::tuple<double, double, double> colors) {
  vtkSmartPointer<vtkPolyData> polyData = vtkPolyData::New();
  vtkSmartPointer<vtkPoints> points = vtkPoints::New();
  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();
  vtkSmartPointer<vtkActor> actor = vtkActor::New();
  vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();

  for (int depth = 0; depth < image3d->getDepth(); depth++) {
    for (int col = 0; col < image3d->getCols(); col++) {
      for (int row = 0; row < image3d->getRows(); row++) {
        if (image3d->getImageAtDepth(depth).at<uchar>(row, col) == 255) {
          vtkIdType pid[1];
          pid[0] = points->InsertNextPoint(col, row, depth);
          vertices->InsertNextCell(1, pid);
        }
      }
    }
  }

  polyData->SetPoints(points);
  polyData->SetVerts(vertices);
  mapper->SetInputData(polyData);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(std::get<0>(colors), std::get<1>(colors),
                                 std::get<2>(colors));

  return actor;
}

void VTKData::initVTKImage() {
  vtkSmartPointer<vtkActor> actor =
      createActorOutOf3dImage(std::make_tuple(0.9, 0.9, 0.9));
  renderer->AddActor(actor);
}

void VTKData::addNextImage(std::tuple<double, double, double> colors) {
  vtkSmartPointer<vtkActor> actor = createActorOutOf3dImage(colors);
  renderer->AddActor(actor);
  renderer->Render();
}
