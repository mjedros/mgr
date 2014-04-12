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
   MainWindow w;
   w.show();
   return a.exec();
}
#else
int main(/*int argc, char *argv[]*/)
{
   try
   {
      std::shared_ptr<OpenCLManager> openCLManager(new OpenCLManager);
      openCLManager->Configure("../Kernels/Kernels.cl", 0, 0);

      ApplicationManager appman(openCLManager);
      appman.DoSth();
   }

   catch (Error &e) { std::cout << e.what() << " error, number= " << e.err() << std::endl; }
}
#endif
