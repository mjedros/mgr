#include "OpenCLManager.h"

#include "Logger.h"
#include <fstream>
#include <iostream>
#include <map>

using namespace std;
using namespace cl;

namespace Mgr {

static Logger &logger = Logger::getInstance();

map<cl_device_type, std::string> DeviceNameToStringMap = {
  { CL_DEVICE_TYPE_GPU, "GPU" },
  { CL_DEVICE_TYPE_CPU, "CPU" },
  { CL_DEVICE_TYPE_ACCELERATOR, "Accelerator" }
};

OpenCLManager::OpenCLManager() {
  try {
    Platform::get(&platforms);
  } catch (Error &error) {
    logger.printError(error);
    throw std::string(error.what());
  }
}

OpenCLManager::~OpenCLManager() {
  logger.printText("OpenCLManager destructor");
}

void OpenCLManager::configure(
    const std::string &kernelFileName,
    const std::pair<unsigned int, unsigned int> &ChosenDevice) {
  try {
    chooseDevice(ChosenDevice.first, ChosenDevice.second);
    logger.printText("Device chosen");
    createContext();
    logger.printText("Context Created");
    readPrograms(kernelFileName);
    logger.printText("Programs Loaded");
    queue = CommandQueue(context, processingDevice);
  } catch (std::string &e) {
    throw std::string("Configure error: " + e);
  } catch (...) {
    throw std::string("Configure error!");
  }
}

void OpenCLManager::readPrograms(const std::string &kernelFileName) {
  ifstream file(kernelFileName);
  if (!file.is_open()) {
    throw std::string("Kernel file not loaded");
  }

  const std::string prog(istreambuf_iterator<char>(file),
                         (istreambuf_iterator<char>()));
  const Program::Sources source =
      Program::Sources(1, make_pair(prog.c_str(), prog.length() + 1));
  program = Program(context, source);
  try {
    program.build({ processingDevice }, "");
  } catch (Error &e) {
    std::string ErrorString(
        "Build error:" +
        program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(processingDevice) +
        e.what());
    logger.printText(ErrorString);
    throw std::string(ErrorString);
  }
}

void OpenCLManager::createContext() { context = Context({ processingDevice }); }

void OpenCLManager::chooseDevice(const unsigned int &platformId,
                                 const unsigned int &DeviceId) {
  std::vector<Device> devices;
  platforms[platformId].getDevices(CL_DEVICE_TYPE_ALL, &devices);
  processingDevice = devices[DeviceId];
}
std::vector<tuple<int, int, std::string>> OpenCLManager::listPlatforms() const {
  std::vector<tuple<int, int, std::string>> devicesInfo;
  for (unsigned int id = 0; id < platforms.size(); id++) {
    std::string name, version;
    platforms[id].getInfo(CL_PLATFORM_NAME, &name);
    platforms[id].getInfo((cl_platform_info)CL_PLATFORM_VERSION, &version);

    std::vector<Device> devices;
    platforms[id].getDevices(CL_DEVICE_TYPE_ALL, &devices);

    cl_device_type deviceType;
    for (unsigned int DeviceId = 0; DeviceId < devices.size(); DeviceId++) {
      devices[DeviceId].getInfo(CL_DEVICE_TYPE, &deviceType);
      devicesInfo.push_back(
          make_tuple(id, DeviceId, DeviceNameToStringMap[deviceType] + "-" +
                                       name + "," + version));
    }
  }
  return devicesInfo;
}
}
