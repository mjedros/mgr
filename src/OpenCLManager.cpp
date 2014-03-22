#include "OpenCLManager.h"
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace cl;
using namespace std;
OpenCLManager::OpenCLManager(int a) { std::cout << "construct Manager" << a<< std::endl; }

OpenCLManager::~OpenCLManager() {}

void OpenCLManager::Configure(std::string kernelFileName) {
  CreateContext();
  ChooseDevice();
  ReadPrograms(kernelFileName);
  queue = CommandQueue(context, processingDevice, 0);
}

void OpenCLManager::ReadPrograms(std::string kernelFileName) {
  std::ifstream file(kernelFileName);
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
    program.build(devices, "");
  }
  catch (Error & e) {
    std::string buildLog =
        program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
    cout << "Build error:" << endl << buildLog << endl;
  }
}

void OpenCLManager::CreateContext() {
  std::vector<Platform> platforms;
  Platform::get(&platforms);
  if (platforms.size() == 1) {
    std::string platformName, platformVersion;
    platforms[0].getInfo((cl_platform_info) CL_PLATFORM_NAME, &platformName);
    platforms[0]
        .getInfo((cl_platform_info) CL_PLATFORM_VERSION, &platformVersion);
    cout << "Only one OpenCL platform detected: " << platformName
         << ", platform version: " << platformVersion << endl;
  }
  cl_context_properties context_properties[3] = {
    CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0
  };
  context = Context(CL_DEVICE_TYPE_GPU, context_properties);
}

void OpenCLManager::ChooseDevice() {
  devices = context.getInfo<CL_CONTEXT_DEVICES>();
  std::string deviceType;
  devices[0].getInfo((cl_device_info) CL_DEVICE_TYPE, &deviceType);
  for(unsigned int i=0 ; i<devices.size();++i) {
      std::string name;
      devices[i].getInfo((cl_device_info) CL_DEVICE_NAME,&name);
      cout<<i<<". Device name: " <<name<<endl;
  }
  cout<< "Choose number"<<endl;
  int number;
  cin>>number;

    std::string deviceName;
    devices[number].getInfo((cl_device_info) CL_DEVICE_NAME, &deviceName);
    processingDevice = devices[number];
    cout << "Chosen device: " << deviceName << endl;
  

}
