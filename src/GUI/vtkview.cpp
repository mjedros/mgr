#include "GUI/vtkview.h"

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include "GUI/VTKData.h"
#include <memory>
#include <mutex>
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

VTKView::VTKView(QObject *parentObj)
  : QVTKWidget(nullptr), ui(new Ui::VTKView), parentObject(parentObj) {
  ui->setupUi(this);
  vtkData = std::unique_ptr<VTKData>(new VTKData());
  GetRenderWindow()->AddRenderer(vtkData->getVTKRenderer());
  renWin = GetRenderWindow();
  renWin->StereoCapableWindowOn();
  iren = GetInteractor();
  connect(this, SIGNAL(showVtkImage()), parentObject, SLOT(showVtkImage()));
}

void VTKView::setImage3d(const std::shared_ptr<Mgr::Image3d> &image) {
  vtkData->setImage3d(image);
}

void VTKView::initImage() {
  vtkData->initVTKImage();
  emit(showVtkImage());
}

void VTKView::renderNewImage(std::tuple<double, double, double> colors) {
  vtkData->addNextImage(colors);
}

void VTKView::render() {
  std::lock_guard<std::mutex> lock(vtkData->rendererMutex);
  renWin->Render();
}

void VTKView::mousePressEvent(QMouseEvent *event) {
  QVTKWidget::mousePressEvent(event);
}

void VTKView::mouseReleaseEvent(QMouseEvent *event) {
  QVTKWidget::mouseReleaseEvent(event);
}

void VTKView::showEvent(QShowEvent *event) {
  std::lock_guard<std::mutex> lock(vtkData->rendererMutex);
  renWin->Render();
  QVTKWidget::showEvent(event);
}

VTKView::~VTKView() {}
