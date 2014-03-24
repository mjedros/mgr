#include "OpenCLManager.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace cl;

OpenCLManager::OpenCLManager() { Platform::get(&platforms); }

OpenCLManager::~OpenCLManager() {}

void OpenCLManager::Configure(std::string kernelFileName,
                              unsigned int platformId, unsigned int deviceId) {
  ChooseDevice(platformId, deviceId);
  CreateContext(platformId);
  ReadPrograms(kernelFileName);
  queue = CommandQueue(context, processingDevice);
}

void OpenCLManager::ReadPrograms(std::string kernelFileName) {
  ifstream file(kernelFileName);
  if (!file.is_open()) {
    cout << "Kernel file not loaded" << endl;
    throw new exception();
  }
  std::string prog(istreambuf_iterator<char>(file),
                   (istreambuf_iterator<char>()));
  Program::Sources source =
      Program::Sources(1, make_pair(prog.c_str(), prog.length() + 1));
  program = Program(context, source);
  try {
    std::vector<Device> devices = {processingDevice};
    program.build(devices, "");
  }
  catch (Error &e) {
    std::string buildLog =
        program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(processingDevice);
    cout << "Build error:" << endl << buildLog << endl;
  }
}

void OpenCLManager::CreateContext(int platformId) {

  cl_context_properties context_properties[3] = {
      CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[platformId])(), 0};
  cl_device_type deviceType;
  processingDevice.getInfo((cl_device_info)CL_DEVICE_TYPE, &deviceType);
  context = Context(deviceType, context_properties);
}

void OpenCLManager::ChooseDevice(const unsigned int platformId,
                                 const unsigned int DeviceId) {

  std::vector<Device> devices;
  platforms[platformId].getDevices(CL_DEVICE_TYPE_ALL, &devices);
  processingDevice = devices[DeviceId];
}

std::vector<std::string> OpenCLManager::ListPlatforms() {
  std::vector<std::string> devicesInfo;
  for (auto &platform : platforms) {
    std::string name, version;
    platform.getInfo((cl_platform_info)CL_PLATFORM_NAME, &name);
    platform.getInfo((cl_platform_info)CL_PLATFORM_VERSION, &version);
    std::vector<Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    cl_device_type deviceType;
    for (unsigned int j = 0; j < devices.size(); j++) {
      devices[j].getInfo((cl_device_info)CL_DEVICE_TYPE, &deviceType);
      std::string deviceT = "";
      devicesInfo.push_back(deviceT + "-" + name + "," + version);
    }
  }
  return devicesInfo;
}
