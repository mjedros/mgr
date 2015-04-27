#include "ApplicationManager.h"

#include "GUI/mainwindow.h"
#include "OpenCLManager.h"
#include <QDebug>
#include <QApplication>
#include <iostream>
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
