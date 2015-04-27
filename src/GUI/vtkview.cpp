#include "GUI/vtkview.h"

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include "GUI/VTKData.h"
#include <memory>
QT_BEGIN_NAMESPACE

class Ui_VTKView {
public:
  void setupUi(QVTKWidget *VTKView) {
    if (VTKView->objectName().isEmpty())
      VTKView->setObjectName(QString::fromUtf8("VTKView"));
    VTKView->resize(800, 600);

    retranslateUi(VTKView);

    QMetaObject::connectSlotsByName(VTKView);
  }

  void retranslateUi(QVTKWidget *VTKView) {
    VTKView->setWindowTitle(QApplication::translate("VTKView", "3d view", 0,
                                                    QApplication::UnicodeUTF8));
  }
};

namespace Ui {
class VTKView : public Ui_VTKView {};
} // namespace Ui

QT_END_NAMESPACE

VTKView::VTKView(std::unique_ptr<QVTKWidget> parent)
  : QVTKWidget(parent.get()), ui(new Ui::VTKView) {
  ui->setupUi(this);
  vtkData = std::unique_ptr<VTKData>(new VTKData());
  GetRenderWindow()->AddRenderer(vtkData->getVTKRenderer());
  renWin = GetRenderWindow();
  renWin->StereoCapableWindowOn();
  iren = GetInteractor();
}

void VTKView::setImage3d(const std::shared_ptr<Mgr::Image3d> &image) {
  vtkData->setImage3d(image);
}

void VTKView::initImage() {
  vtkData->initVTKImage();
  renWin->Render();
}

void VTKView::renderNewImage(std::tuple<double, double, double> colors) {
  vtkData->addNextImage(colors);
  renWin->Render();
}

VTKView::~VTKView() {}
