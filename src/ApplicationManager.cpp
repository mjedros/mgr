#include "ApplicationManager.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using CLProcessingImage::ProcessingImage;
using std::string;
using std::shared_ptr;
using std::unique_ptr;

void ApplicationManager::sliderValueChanged(const int &value,
                                            const QString &title) {
    if (title.startsWith("Depth"))
        showWindows(value);
    else if (title.startsWith("Cols"))
        showCols(value);
}
void ApplicationManager::saveMovie(const Image3d &img3d,
                                   const std::string &filename) {

    VideoWriter videowriter(filename, CV_FOURCC('D', 'I', 'V', 'X'), 50,
                            cv::Size(img3d.getRows(), img3d.getCols()), false);
    if (!videowriter.isOpened()) {
        std::cout << "Could not open the output video for write: " << std::endl;
        return;
    }
    for (int j = 0; j < img3d.getDepth(); j++) {
        videowriter << img3d.getImageAtDepth(j);
    }
}

void ApplicationManager::showWindows(const int &depth) {
    unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
    img->SetImageToProcess(image3d->getImageAtDepth(depth).clone());
    originalWindow->draw(img->GetImage());
    img->Binarize(70, 255);
    img->SetStructuralElement(CLProcessingImage::ELLIPSE, { 4, 3, 2 });
    img->Dilate();
    img->Erode();
    processedWindow->draw(img->GetImage());
    processedWindow->update();
    originalWindow->update();
}

void ApplicationManager::showCols(const int &col) {
    unique_ptr<ProcessingImage> img(new ProcessingImage(openCLManager));
    img->SetImageToProcess(image3d->getImageAtCol(col));
    colsOriginal->draw(img->GetImage());
    img->Binarize(70, 255);
    img->SetStructuralElement(CLProcessingImage::ELLIPSE, { 4, 3, 2 });
    img->Dilate();
    colsProcessed->draw(img->GetImage());
    colsProcessed->update();
    colsOriginal->update();
}

void ApplicationManager::setMaxValues() {
    originalWindow->setMaxValue(image3d->getDepth() - 1);
    colsOriginal->setMaxValue(image3d->getCols() - 1);
}

ApplicationManager::ApplicationManager(
    shared_ptr<OpenCLManager> openCLManagerPtr)
    : openCLManager(std::move(openCLManagerPtr)) {}

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

void ApplicationManager::LoadDir3dImage(const string &Directory) {
    auto files = readDir(Directory);
    cv::Mat image2d = cv::imread(Directory + *files.begin());
    cv::cvtColor(image2d, image2d, CV_BGR2GRAY);
    image3d = std::unique_ptr<Image3d>(new Image3d(files.size(), image2d));
    for (auto it = files.begin(); it != files.end(); ++it) {
        image2d = cv::imread(Directory + *it);
        cv::cvtColor(image2d, image2d, CV_BGR2GRAY);
        image3d->setImageAtDepth(std::distance(files.begin(), it), image2d);
    }
    saveMovie(*image3d, "../Data/movie2.avi");
}

void ApplicationManager::LoadFile3dImage(const std::string &filename) {

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

void ApplicationManager::InitWindows(const OBJECT &object,
                                     const std::string &name) {
    switch (object) {
    case OBJECT::DIRECTORY:
        LoadDir3dImage(name);
        break;
    case OBJECT::MOVIE:
        LoadFile3dImage(name);
        break;
    }
}

void ApplicationManager::ShowImages() {
    originalWindow.reset(new cvImageWindow("Depth Original", this));
    processedWindow.reset(new cvImageWindow("Depth Processed", this));
    colsOriginal.reset(new cvImageWindow("Cols Original", this));
    colsProcessed.reset(new cvImageWindow("Cols Processed", this));
    setMaxValues();
    showWindows(image3d->getDepth() / 2);
    showCols(image3d->getCols() / 2);
}
