#ifndef OPENCLMANAGER_H_
#define OPENCLMANAGER_H_
#define LOGGING_ON
#ifdef LOGGING_ON
#define LOG(x) std::cout<<x<<std::endl;
#else
#define LOG(x) ;
#endif

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <memory>
#include <string>

class OpenCLManager
{
 private:
   cl::Device processingDevice;
   cl::Buffer outBuffer;
   std::vector<cl::Platform> platforms;

   void CreateContext(const unsigned int platformId);
   void ReadPrograms(std::string kernelFileName);

 public:
   cl::Context context;
   cl::CommandQueue queue;
   cl::Program program;
   void Configure(const std::string kernelFileName,
                  const std::pair<unsigned int, unsigned int> ChosenDevice);
   std::vector<std::tuple<int, int, std::string>> ListPlatforms();
   void ChooseDevice(const unsigned int platformId, const unsigned int DeviceId);
   OpenCLManager();

   virtual ~OpenCLManager();
};

#endif /* OpenCLManager_H_ */
