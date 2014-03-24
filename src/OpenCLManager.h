#ifndef OPENCLMANAGER_H_
#define OPENCLMANAGER_H_

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
//#include <opencv2/opencv.hpp>
#include <memory>
#include <string>

class OpenCLManager {
private:
  cl::Device processingDevice;
  cl::Buffer outBuffer;
  std::vector<cl::Platform> platforms;

  void CreateContext(int platformId);
  void ReadPrograms(std::string kernelFileName);

public:
  cl::Context context;
  cl::CommandQueue queue;
  cl::Program program;
  void Configure(std::string kernelFileName, unsigned int platformId,
                 unsigned int deviceId);
  std::vector<std::string> ListPlatforms();
  void ChooseDevice(const unsigned int platformId, const unsigned int DeviceId);
  OpenCLManager();
  virtual ~OpenCLManager();
};

#endif /* OpenCLManager_H_ */
