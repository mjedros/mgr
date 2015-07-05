#include "ApplicationManager.h"

#include "GUI/mainwindow.h"
#include "Logger.h"
#include "OpenCLManager.h"
#include <QDebug>
#include <QApplication>
#include <iostream>
#include <X11/Xlib.h>
using namespace cl;

static Mgr::Logger &logger = Mgr::Logger::getInstance();

int main(int argc, char *argv[]) {
  XInitThreads();
  QApplication a(argc, argv);
  try {
    MainWindow w;
    w.show();
    return a.exec();
  } catch (std::string &e) {
    logger.printLine(" error, number= " + e);
  } catch (cl::Error &e) {
    logger.printLine(" error, number= " + (int)e.err());
  }
  std::terminate();
}
