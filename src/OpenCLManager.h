#pragma once

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <memory>
#include <string>
namespace Mgr {
class Logger;
class OpenCLManager {
private:
  cl::Device processingDevice;
  std::vector<cl::Platform> platforms;
  void createContext();
  void readPrograms(const std::string &kernelFileName);

public:
  cl::Context context;
  cl::CommandQueue queue;
  cl::Program program;
  void configure(const std::string &kernelFileName,
                 const std::pair<unsigned int, unsigned int> &ChosenDevice);
  std::vector<std::tuple<int, int, std::string>> listPlatforms() const;
  void chooseDevice(const unsigned int &platformId,
                    const unsigned int &DeviceId);
  OpenCLManager();
  ~OpenCLManager();
};
}
