#ifndef VTKDATA_H
#define VTKDATA_H
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleUnicam.h>
#include <vtkVersion.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkOutlineFilter.h>

#include <vtkEventQtSlotConnect.h> // połączenie VTK->Qt
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <QMouseEvent>
#include <QPoint>
#include <vtkPolyData.h>
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkImplicitModeller.h>
#include <vtkContourFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPNGWriter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkRenderLargeImage.h>
#include <QMessageBox>
#include <vtkGenericDataObjectWriter.h>
class VTKData {
    vtkRenderer *ren;

  public:
    VTKData();
    vtkRenderer * getVTKRenderer() const { return ren; }
};

#endif // VTKDATA_H
