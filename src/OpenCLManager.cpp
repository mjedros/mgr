#include "OpenCLManager.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace cl;

OpenCLManager::OpenCLManager() { Platform::get(&platforms); }

OpenCLManager::~OpenCLManager() { LOG("OpenCLManager destructor"); }

void OpenCLManager::Configure(const std::string kernelFileName,
                              const std::pair<unsigned int, unsigned int> ChosenDevice)
{
   try
   {
      ChooseDevice(ChosenDevice.first, ChosenDevice.second);
      CreateContext(ChosenDevice.first);
      ReadPrograms(kernelFileName);
      queue = CommandQueue(context, processingDevice);
   }
   catch (std::string &e) { throw std::string("Configure error: " + e ); }
}

void OpenCLManager::ReadPrograms(std::string kernelFileName)
{
   ifstream file(kernelFileName);
   if (!file.is_open())
   {
      throw std::string("Kernel file not loaded");
   }

   std::string prog(istreambuf_iterator<char>(file), (istreambuf_iterator<char>()));
   Program::Sources source = Program::Sources(1, make_pair(prog.c_str(), prog.length() + 1));
   program = Program(context, source);
   try
   {
      std::vector<Device> devices = {processingDevice};
      program.build(devices, "");
   }
   catch (Error &e)
   {
      std::string buildLog = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(processingDevice);
      throw std::string("Build error:" + buildLog + e.what());
   }
}

void OpenCLManager::CreateContext(const unsigned int platformId)
{

   cl_context_properties context_properties[3] = {
       CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[platformId])(), 0};
   cl_device_type deviceType;
   processingDevice.getInfo((cl_device_info)CL_DEVICE_TYPE, &deviceType);
   context = Context(deviceType, context_properties);
}

void OpenCLManager::ChooseDevice(const unsigned int platformId, const unsigned int DeviceId)
{
   std::vector<Device> devices;
   platforms[platformId].getDevices(CL_DEVICE_TYPE_ALL, &devices);
   processingDevice = devices[DeviceId];
}

std::vector<tuple<int, int, std::string>> OpenCLManager::ListPlatforms()
{
   std::vector<tuple<int, int, std::string>> devicesInfo;
   for (unsigned int id = 0; id < platforms.size(); id++)
   {
      std::string name, version;
      platforms[id].getInfo((cl_platform_info)CL_PLATFORM_NAME, &name);
      platforms[id].getInfo((cl_platform_info)CL_PLATFORM_VERSION, &version);
      std::vector<Device> devices;
      platforms[id].getDevices(CL_DEVICE_TYPE_ALL, &devices);
      cl_device_type deviceType;
      for (unsigned int DeviceId = 0; DeviceId < devices.size(); DeviceId++)
      {
         devices[DeviceId].getInfo((cl_device_info)CL_DEVICE_TYPE, &deviceType);
         std::string deviceT = "";
         devicesInfo.push_back(make_tuple(id, DeviceId, deviceT + "-" + name + "," + version));
      }
   }
   return devicesInfo;
}
