#ifndef OPENCLMANAGER_H_
#define OPENCLMANAGER_H_

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
//#include <opencv2/opencv.hpp>
#include <memory>
#include <string>

class OpenCLManager {
private:

  std::vector<cl::Device> devices;
  cl::Device processingDevice;
  cl::Buffer outBuffer;
  void CreateContext();
  void ChooseDevice();
  void ReadPrograms(std::string kernelFileName);

public:
  OpenCLManager(int);
  cl::Context context;
  cl::CommandQueue queue;
  cl::Program program;
  void Configure(std::string kernelFileName);
  virtual ~OpenCLManager();
};

#endif /* OpenCLManager_H_ */
