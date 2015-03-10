
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_VTKView
{
public:
    QVTKWidget *vtkwidget;

    void setupUi(QWidget *VTKView)
    {
        if (VTKView->objectName().isEmpty())
            VTKView->setObjectName(QString::fromUtf8("VTKView"));
        VTKView->resize(764, 492);
        vtkwidget = new QVTKWidget(VTKView);
        vtkwidget->setObjectName(QString::fromUtf8("vtkwidget"));
        vtkwidget->setGeometry(QRect(10, 10, 741, 461));

        retranslateUi(VTKView);

        QMetaObject::connectSlotsByName(VTKView);
    } // setupUi

    void retranslateUi(QWidget *VTKView)
    {
        VTKView->setWindowTitle(QApplication::translate("VTKView", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class VTKView: public Ui_VTKView {};
} // namespace Ui

QT_END_NAMESPACE
#include "vtkview.h"
VTKView::VTKView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VTKView)
{
    ui->setupUi(this);
}

VTKView::~VTKView()
{
    delete ui;
}
