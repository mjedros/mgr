#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "../src/OpenCLManager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    OpenCLManager openCLManager;
    std::vector<std::string> ListPlatforms = openCLManager.ListPlatforms();
    for(unsigned int i=0;i<ListPlatforms.size();++i){
        std::cout<<ListPlatforms[i]<<std::endl;
    }
    return a.exec();
}

