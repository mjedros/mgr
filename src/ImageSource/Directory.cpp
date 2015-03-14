#include "Directory.h"
#include <opencv2/opencv.hpp>
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
Directory::Directory(const std::string &directory) : directory(directory) {}

void Directory::Start() {
  files = readDir(directory);
  it = files.begin();
}

void Directory::Stop() {}

cv::Mat Directory::Get() {
  if (it != files.end())
    return cv::imread(directory + *it++);
  else
    return cv::Mat();
}
}
