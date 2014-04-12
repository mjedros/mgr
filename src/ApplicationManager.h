#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "SourceFactory.h"

class ApplicationManager
{
private:
    OpenCLManager * openCLManager;
public:
    ApplicationManager(OpenCLManager *openCLManager);
    void DoSth();
};

#endif // APPLICATIONMANAGER_H
