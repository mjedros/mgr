#include "OpenCLManager.h"

#include "Logger.h"
#include "include/Paths.h"
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
  logger.printLine("OpenCLManager destructor");
}

void OpenCLManager::configure(
    const std::string &kernelFileName,
    const std::pair<unsigned int, unsigned int> &ChosenDevice) {
  try {
    chooseDevice(ChosenDevice.first, ChosenDevice.second);
    logger.printLine("Device chosen");
    createContext();
    logger.printLine("Context Created");
    readPrograms(kernelFileName);
    logger.printLine("Programs Loaded");
    queue = CommandQueue(context, processingDevice, CL_QUEUE_PROFILING_ENABLE);
  } catch (std::string &e) {
    throw std::string("Configure error: " + e);
  } catch (...) {
    throw std::string("Configure error!");
  }
  configured = true;
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
    const std::string includeDir = "-I" + std::string(KERNELS_DIR);
    program.build({ processingDevice }, includeDir.c_str());
  } catch (Error &e) {
    std::string ErrorString(
        "Build error:" +
        program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(processingDevice) +
        e.what());
    logger.printLine(ErrorString);
    throw std::string(ErrorString);
  }
}

void OpenCLManager::createContext() {
  std::vector<Device> devices = { processingDevice };
  context = Context(devices);
}

void OpenCLManager::chooseDevice(const unsigned int &platformId,
                                 const unsigned int &DeviceId) {
  std::vector<Device> devices;
  platforms[platformId].getDevices(CL_DEVICE_TYPE_ALL, &devices);
  processingDevice = devices[DeviceId];
}
void OpenCLManager::printDeviceInfo(std::string name, unsigned int DeviceId,
                                    std::vector<Device> devices) const {
  std::string info;
  devices[DeviceId].getInfo(CL_DEVICE_EXTENSIONS, &info);
  if (info.find("cl_khr_3d_image_writes") != std::string::npos)
    std::cout << name << " Supports 3d images !" << std::endl;
  else
    std::cout << name << " Does NOT support 3d images !" << std::endl;
  std::size_t maxNDRange;
  std::size_t NDRanges[3];
  devices[DeviceId].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &maxNDRange);
  devices[DeviceId].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &NDRanges);

  std::cout << "Max NDRange: " << maxNDRange << " " << NDRanges[0] << ","
            << NDRanges[1] << "," << NDRanges[2] << std::endl;
}

PlatformList OpenCLManager::listPlatforms() const {
  PlatformList devicesInfo;
  for (unsigned int id = 0; id < platforms.size(); id++) {
    std::string name, version;
    platforms[id].getInfo(CL_PLATFORM_NAME, &name);
    platforms[id].getInfo(CL_PLATFORM_VERSION, &version);

    std::vector<Device> devices;
    platforms[id].getDevices(CL_DEVICE_TYPE_ALL, &devices);

    cl_device_type deviceType;
    for (unsigned int DeviceId = 0; DeviceId < devices.size(); DeviceId++) {
      devices[DeviceId].getInfo(CL_DEVICE_TYPE, &deviceType);
      printDeviceInfo(name, DeviceId, devices);
      devicesInfo.push_back(
          std::make_tuple(id, DeviceId, DeviceNameToStringMap[deviceType] +
                                            "-" + name + "," + version));
    }
  }
  return devicesInfo;
}
}
