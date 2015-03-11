#ifndef VTKVIEW_H
#define VTKVIEW_H
#include <vtkRenderWindow.h>
#include <QVTKInteractor.h>
#include <QWidget>

namespace Ui {
class VTKView;
}
class VTKData;
class VTKView : public QWidget {
    Q_OBJECT

  public:
    explicit VTKView(QWidget *parent = 0);
    ~VTKView();

  private:
    Ui::VTKView *ui;
    vtkRenderWindow* renWin;
    VTKData *vtkData;
    QVTKInteractor* iren;
};

#endif // VTKVIEW_H
