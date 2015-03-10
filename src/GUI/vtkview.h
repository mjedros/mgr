#ifndef VTKVIEW_H
#define VTKVIEW_H

#include <QWidget>

namespace Ui {
class VTKView;
}

class VTKView : public QWidget
{
    Q_OBJECT

public:
    explicit VTKView(QWidget *parent = 0);
    ~VTKView();

private:
    Ui::VTKView *ui;
};

#endif // VTKVIEW_H
