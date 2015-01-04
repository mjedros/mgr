#include "ProcessingImage.h"
using namespace Mgr;
using namespace cv;
using namespace cl;

std::map<std::string, StructuralElement> StrElementMap = {
    { "Ellipse", ELLIPSE }, { "Cross", CROSS }, { "Rectangle", RECTANGLE }
};

void
ProcessingImage::performMorphologicalOperation(const std::string &Operation) {
    const std::string kernelName = Operation + structuralElementType;
    Kernel kernel(openCLManager->program, kernelName.c_str());
    const ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
    const Image2D image_in(openCLManager->context,
                           CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                           image.cols, image.rows, 0, image.data);
    Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, format,
                      image.cols, image.rows);
    setStructuralElementArgument(kernel);
    process(kernel, image_in, image_out);
}

ProcessingImage::ProcessingImage(
    std::shared_ptr<OpenCLManager> openCLManagerPtr)
    : openCLManager(std::move(openCLManagerPtr)) {
    origin[0] = origin[1] = origin[2] = 0;
}

Mat ProcessingImage::getImage() { return image; }
void ProcessingImage::setImageToProcess(cv::Mat img) {
    image.release();
    image = img;
    region[0] = image.cols;
    region[1] = image.rows;
    region[2] = 1;
    if (image.cols == image.rows)
        localRange = { 16, 4 };
    else if (image.cols < image.rows)
        localRange = { 10, 16 };
    else
        localRange = { 16, 10 };
}

void ProcessingImage::dilate() { performMorphologicalOperation("Dilate"); }

void ProcessingImage::erode() { performMorphologicalOperation("Erode"); }

void ProcessingImage::contour() {
    erode();
    cv::Mat temp = image.clone();
    dilate();
    image -= temp;
}

void ProcessingImage::skeletonize() {
    cv::Mat skel(image.size(), CV_8U, cv::Scalar(0));
    cv::Mat eroded;
    do {
        cv::Mat img = image.clone();
        erode();
        eroded = image.clone();
        dilate();
        img -= image;

        cv::bitwise_or(skel, img, skel);
        image = eroded;
    } while (!(cv::countNonZero(eroded) == 0));
    image = skel;
}

void ProcessingImage::setStructuralElement(const std::string &element,
                                           const std::vector<float> &params) {
    if (StrElementMap.find(element.c_str()) == StrElementMap.end()) {
        throw std::string("Not existing structural element");
    }

    structuralElementType = element;
    structuralElementParams = params;
}

void ProcessingImage::setStructuralElementArgument(cl::Kernel &kernel) {
    switch (StrElementMap[structuralElementType.c_str()]) {
    case ELLIPSE: {
        const cl_float3 ellipseParams =
            (cl_float3) { { structuralElementParams[0],
                            structuralElementParams[1],
                            structuralElementParams[2] } };
        kernel.setArg(2, ellipseParams);
        break;
    }
    case RECTANGLE:
    case CROSS: {
        const cl_int2 rectParams =
            (cl_int2) { {(int)structuralElementParams[0],
                         (int)structuralElementParams[1] } };
        kernel.setArg(2, rectParams);
        break;
    }
    default:
        break;
    }
}

void ProcessingImage::binarize(const unsigned int &minimum,
                               const unsigned int &maximum) {
    cl::Kernel kernel = cl::Kernel(openCLManager->program, "Binarize");

    const cl::ImageFormat format(CL_INTENSITY, CL_UNSIGNED_INT8);
    const cl::ImageFormat formatOut(CL_INTENSITY, CL_UNSIGNED_INT8);
    const cl::Image2D image_in(openCLManager->context,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                               image.cols, image.rows, 0, image.data);
    cl::Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, formatOut,
                          image.cols, image.rows);
    kernel.setArg(2, minimum);
    kernel.setArg(3, maximum);
    process(kernel, image_in, image_out);
}

void ProcessingImage::process(cl::Kernel &kernel, const cl::Image2D &image_in,
                              cl::Image2D &image_out) {
    kernel.setArg(0, image_in);
    kernel.setArg(1, image_out);
    openCLManager->queue.enqueueNDRangeKernel(
        kernel, cl::NullRange, cl::NDRange(image.cols, image.rows), localRange,
        NULL, NULL);
    openCLManager->queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0,
                                          0, image.data);
}
