#include "ApplicationManager.h"
#include <opencv2/opencv.hpp>
#include "SourceFactory.h"
#include "Normalization.h"
using namespace cv;
using namespace Mgr;
using std::string;
using std::shared_ptr;
using std::unique_ptr;

typedef void (ProcessingImage::*pointerToProcessingMethodType)();
const std::map<OPERATION, pointerToProcessingMethodType>
OperationToMethodPointerMap = {
    { OPERATION::DILATION, &ProcessingImage::dilate },
    { OPERATION::EROSION, &ProcessingImage::erode },
    { OPERATION::CONTOUR, &ProcessingImage::contour },
    { OPERATION::SKELETONIZATION, &ProcessingImage::skeletonize }
};
class Processing3dImage {
  public:
    virtual void
    process(std::shared_ptr<Image3d> image,
            const shared_ptr<ProcessingImage> img,
            const pointerToProcessingMethodType pointerToMethod) = 0;
};
class ProcessCols : public Processing3dImage {
  public:
    void process(std::shared_ptr<Image3d> image,
                 std::shared_ptr<ProcessingImage> img,
                 pointerToProcessingMethodType operation) {
        for (auto i = 0; i < image->getCols(); i++) {
            img->setImageToProcess(image->getImageAtCol(i).clone());
            (img.get()->*operation)();
            image->setImageAtCol(i, img->getImage());
        }
    }
};
class ProcessDepth : public Processing3dImage {
  public:
    void process(std::shared_ptr<Image3d> image,
                 const std::shared_ptr<ProcessingImage> img,
                 const pointerToProcessingMethodType operation) {
        for (auto i = 0; i < image->getDepth(); i++) {
            img->setImageToProcess(image->getImageAtDepth(i).clone());
            (img.get()->*operation)();
            image->setImageAtDepth(i, img->getImage());
        }
    }
};

namespace {
#include <dirent.h>
std::set<string> readDir(const string &directory) {
    std::set<string> filenames;
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
void saveMovie(const Image3d &img3d, const std::string &filename) {

    VideoWriter videowriter(filename, CV_FOURCC('D', 'I', 'V', 'X'), 50,
                            cv::Size(img3d.getRows(), img3d.getCols()), false);
    if (!videowriter.isOpened()) {
        std::cout << "Could not open the output video for write " << std::endl;
        return;
    }
    for (int j = 0; j < img3d.getDepth(); j++) {
        videowriter << img3d.getImageAtDepth(j);
    }
}
}
void ApplicationManager::loadDir3dImage(const string &Directory) {
    auto files = readDir(Directory);
    cv::Mat image2d = cv::imread(Directory + *files.begin());
    cv::cvtColor(image2d, image2d, CV_BGR2GRAY);
    image3d = std::unique_ptr<Image3d>(new Image3d(files.size(), image2d));
    for (auto it = files.begin(); it != files.end(); ++it) {
        image2d = cv::imread(Directory + *it);
        cv::cvtColor(image2d, image2d, CV_BGR2GRAY);
        image3d->setImageAtDepth(std::distance(files.begin(), it), image2d);
    }
}

void ApplicationManager::loadFile3dImage(const string &filename) {

    unique_ptr<IImageSource> imageSource =
        SourceFactory::GetImageSource(VideoFile, filename);
    imageSource->Start();
    std::vector<cv::Mat> matVector;
    for (Mat im = imageSource->Get(); !im.empty(); im = imageSource->Get()) {
        cv::cvtColor(im, im, CV_BGR2GRAY);
        matVector.push_back(im);
    }
    image3d.reset(new Image3d(matVector.size(), *matVector.begin()));
    for (auto it = matVector.begin() + 1; it < matVector.end(); it++) {
        image3d->setImageAtDepth(std::distance(matVector.begin(), it), *it);
    }
}
void ApplicationManager::process(const OPERATION &operation,
                                 const string &structuralElement) {
    cv::waitKey(1);
    shared_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
    img->setStructuralElement(structuralElement, { 3, 2, 2 });
    std::unique_ptr<Processing3dImage> processing3dImage;
    processing3dImage = std::unique_ptr<ProcessCols>(new ProcessCols);
    processing3dImage->process(processedImage3d, img,
                               OperationToMethodPointerMap.at(operation));
}
void ApplicationManager::init(const OBJECT &object, const string &name) {
    switch (object) {
    case OBJECT::DIRECTORY:
        loadDir3dImage(name);
        break;
    case OBJECT::MOVIE:
        loadFile3dImage(name);
        break;
    }
}

void ApplicationManager::initProcessedImage(const unsigned int &minumum,
                                            const unsigned int &maximum ) {
    processedImage3d.reset(new Image3d(*image3d));
    for (auto i = 0; i < processedImage3d->getDepth(); i++) {
        unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
        img->setImageToProcess(processedImage3d->getImageAtDepth(i).clone());
        img->binarize(minumum, maximum);
        processedImage3d->setImageAtDepth(i, img->getImage());
    }
}

void ApplicationManager::normalizeOriginalImage() { normalize(image3d); }
