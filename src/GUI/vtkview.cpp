#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "QVTKWidget.h"
#include "VTKData.h"
#include <memory>
QT_BEGIN_NAMESPACE

class Ui_VTKView {
public:
  std::unique_ptr<QVTKWidget> vtkwidget;

  void setupUi(QWidget *VTKView) {
    if (VTKView->objectName().isEmpty())
      VTKView->setObjectName(QString::fromUtf8("VTKView"));
    VTKView->resize(512, 512);
    vtkwidget = std::unique_ptr<QVTKWidget>(new QVTKWidget(VTKView));
    vtkwidget->setObjectName(QString::fromUtf8("vtkwidget"));
    vtkwidget->setGeometry(QRect(0, 0, 512, 512));

    retranslateUi(VTKView);

    QMetaObject::connectSlotsByName(VTKView);
  } // setupUi

  void retranslateUi(QWidget *VTKView) {
    VTKView->setWindowTitle(QApplication::translate("VTKView", "3d view", 0,
                                                    QApplication::UnicodeUTF8));
  } // retranslateUi
};

namespace Ui {
class VTKView : public Ui_VTKView {};
} // namespace Ui

QT_END_NAMESPACE
#include "vtkview.h"
VTKView::VTKView(std::unique_ptr<QWidget> parent)
  : QWidget(parent.get()), ui(new Ui::VTKView) {
  ui->setupUi(this);
  vtkData = std::unique_ptr<VTKData>(new VTKData());
  ui->vtkwidget->GetRenderWindow()->AddRenderer(vtkData->getVTKRenderer());
  renWin = ui->vtkwidget->GetRenderWindow();
  iren = ui->vtkwidget->GetInteractor();
}

void VTKView::setImage3d(const std::shared_ptr<Mgr::Image3d> &image) {
  vtkData->setImage3d(image);
}

VTKView::~VTKView() {}
