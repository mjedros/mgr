#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H
#include "cvImageWindow.h"
#include "ProcessingImage.h"
#include "OpenCLManager.h"
#include "SourceFactory.h"

class ApplicationManager
{
 private:
   const std::shared_ptr<OpenCLManager> openCLManager;
   std::string sourceFilename;

 public:
   ApplicationManager(const std::shared_ptr<OpenCLManager> &openCLManager);
   void SetFileToProcess(std::string filename);
   void DoSth();
};

#endif // APPLICATIONMANAGER_H
