#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "SourceFactory.h"

class ApplicationManager
{
 private:
   const std::shared_ptr<OpenCLManager> openCLManager;

 public:
   ApplicationManager(const std::shared_ptr<OpenCLManager> &openCLManager);
   void DoSth();
};

#endif // APPLICATIONMANAGER_H
