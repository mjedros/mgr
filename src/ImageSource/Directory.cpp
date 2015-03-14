#include "Directory.h"
#include <opencv2/opencv.hpp>
#include <vtkDICOMImageReader.h>
#include <vtkSmartPointer.h>
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
  cv::Mat matImage;
  vtkSmartPointer<vtkDICOMImageReader> reader = vtkDICOMImageReader::New();
  reader->SetFileName(filename.c_str());
  reader->Update();
  return matImage;
}

Directory::Directory(const std::string &directory) : directory(directory) {}

void Directory::Start() {
  files = readDir(directory);
  it = files.begin();
  if (it->find(".dcm"))
    type = DICOM;
  else
    type = OTHER;
}

void Directory::Stop() {}

cv::Mat Directory::Get() {
  if (it != files.end()) {
    if (type == OTHER)
      return cv::imread(directory + *it++);
    else
      return getDicomImage(directory + *it++);
  } else
    return cv::Mat();
}
}
