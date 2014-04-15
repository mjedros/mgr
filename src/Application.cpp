//#define QT_NOT_DEFINED
#ifndef QT_NOT_DEFINED
#include "../Gui/mainwindow.h"
#include <QApplication>
#endif
#include <iostream>
#include "OpenCLManager.h"

#include "ApplicationManager.h"
using namespace cl;

#ifndef QT_NOT_DEFINED
int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   try
   {
      MainWindow w;
      w.show();
      return a.exec();
   }
   catch (std::string &e) { LOG(" error, number= " + e); }
   catch (cl::Error &e) { LOG(" error, number= " + e.err()); }
   std::terminate();

}
#else
int main(/*int argc, char *argv[]*/)
{
   try
   {
      std::shared_ptr<OpenCLManager> openCLManager(new OpenCLManager);
      openCLManager->Configure("../Kernels/Kernels2.cl", std::make_pair(0, 0));

      ApplicationManager appman(openCLManager);
      appman.DoSth();
   }
   catch (std::string &e) { LOG(" error, number= " + e); }
   catch (cl::Error &e) { LOG(" error, number= " + e.err()); }
}
#endif
