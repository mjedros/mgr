
#include "GUI/mainwindow.h"
#include <QApplication>

#include <iostream>
#include "OpenCLManager.h"
#include "ApplicationManager.h"
#include <QDebug>
using namespace cl;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    try {
        MainWindow w;
        w.show();
        return a.exec();
    } catch (std::string &e) {
        LOG(" error, number= " + e);
    } catch (cl::Error &e) {
        LOG(" error, number= " + (int)e.err());
    }
    std::terminate();
}
