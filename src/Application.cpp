//#define QT_NOT_DEFINED
#ifndef QT_NOT_DEFINED
#include "../Gui/mainwindow.h"
#include <QApplication>
#endif
#include <iostream>
#include "OpenCLManager.h"
#include "ApplicationManager.h"
#include "GUI/wxImagesProcessing.h"
#include <wx/wx.h>
using namespace cl;

class wxWidgetsApp : public wxApp
{
public:

    wxWidgetsApp();
    virtual bool OnInit();
};

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
   catch (cl::Error &e) { LOG(" error, number= " + (int)e.err()); }
   std::terminate();
}
#else
IMPLEMENT_APP(wxWidgetsApp)
int App(/*int argc, char *argv[]*/)
{
   try
   {

      std::shared_ptr<OpenCLManager> openCLManager(new OpenCLManager);
      std::vector<std::tuple<int, int, std::string>> ListPlatforms = openCLManager->ListPlatforms();
      {
         std::vector<std::string> ChoosePlatform;
         std::for_each(
             ListPlatforms.begin(), ListPlatforms.end(),
             [&](decltype(*ListPlatforms.begin()) & platform)
             { std::cout << std::get<1>(platform) << (std::get<2>(platform)) << std::endl; });

         for (auto &allplatforms : ChoosePlatform)
         {
            std::cout << allplatforms << std::endl;
         }
      }
      int a;
      std::cin >> a;
      openCLManager->Configure("../Kernels/Kernels.cl", std::make_pair(a, a));

      ApplicationManager appman(openCLManager);
      appman.SetFileToProcess("/home/michal/mgr/Data/768x576.avi");
      appman.DoSth();
   }
   catch (std::string &e) { LOG(" error, number= " + e); }
   catch (cl::Error &e) { LOG(" error, number= " + (int)e.err()); }
   return 0;
}
#endif


wxWidgetsApp::wxWidgetsApp()
{

}

bool wxWidgetsApp::OnInit()
{
    GUI *gui = new GUI((wxWindow*)NULL);
    gui->Show();
    return true;
}
