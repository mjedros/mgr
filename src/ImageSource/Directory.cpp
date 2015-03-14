#include "Directory.h"
#include <opencv2/opencv.hpp>
#include <vtkDICOMImageReader.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
namespace Mgr {
namespace {
#include <dirent.h>
std::set<std::string> readDir(const std::string &directory) {
  std::set<std::string> filenames;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(directory.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (strlen(ent->d_name) > 2)
        filenames.emplace(ent->d_name);
    }
    closedir(dir);
  } else {
    perror("could not open directory");
  }
  return filenames;
}
}
cv::Mat Directory::getDicomImage(const std::string &filename) {
  vtkSmartPointer<vtkDICOMImageReader> reader = vtkDICOMImageReader::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
  vtkSmartPointer<vtkImageData> vtkImage = vtkImageData::New();
  vtkImage = reader->GetOutput();
  int dims[3];
  vtkImage->GetDimensions(dims);
  return cv::Mat(dims[0], dims[1], CV_8UC4, vtkImage->GetScalarPointer());
}

Directory::Directory(const std::string &directory) : directory(directory) {}

void Directory::Start() {
  files = readDir(directory);
  it = files.begin();
  if (it->find(".dcm") != std::string::npos)
    type = DICOM;
  else
    type = OTHER;
}

void Directory::Stop() {}

cv::Mat Directory::Get() {
  if (it != files.end()) {
    if (type == OTHER)
      return cv::imread(directory + *it++);
    else {
      return getDicomImage(directory + *it++);
    }
  } else
    return cv::Mat();
}
}
