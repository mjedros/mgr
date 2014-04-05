#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "SourceFactory.h"

class ApplicationManager
{
    OpenCLManager openCLManager;
public:
    ApplicationManager(OpenCLManager &);
    void DoSth();
};

#endif // APPLICATIONMANAGER_H
