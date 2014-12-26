#include "ProcessingImage.h"
using namespace CLProcessingImage;
using namespace cv;
using namespace cl;

std::map<StructuralElement, std::string> StrElementMap = {
    { ELLIPSE, "Ellipse" }, { CROSS, "Cross" }, { RECTANGLE, "Rectangle" }
};

void
ProcessingImage::PerformMorphologicalOperation(const std::string &Operation) {
    std::string kernelName = Operation + StrElementMap[structuralElementType];
    Kernel kernel(openCLManager->program, kernelName.c_str());
    ImageFormat format(CL_LUMINANCE, CL_UNORM_INT8);
    Image2D image_in(openCLManager->context,
                     CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                     image.cols, image.rows, 0, image.data);
    Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, format,
                      image.cols, image.rows);
    SetStructuralElementArgument(kernel);
    Process(kernel, image_in, image_out);
}

ProcessingImage::ProcessingImage(
    std::shared_ptr<OpenCLManager> openCLManagerPtr)
    : openCLManager(std::move(openCLManagerPtr)) {
    origin[0] = origin[1] = origin[2] = 0;
}

Mat ProcessingImage::GetImage() { return image; }
void ProcessingImage::SetImageToProcess(cv::Mat img) {
    image.release();
    image = img;
    region[0] = image.cols;
    region[1] = image.rows;
    region[2] = 1;
    if (image.cols == image.rows)
        localRange = { 16, 4 };
    else if (image.cols < image.rows)
        localRange = { 1, 4 };
    else
        localRange = { 4, 1 };
}

void ProcessingImage::Dilate() { PerformMorphologicalOperation("Dilate"); }

void ProcessingImage::Erode() { PerformMorphologicalOperation("Erode"); }

void ProcessingImage::Contour() {
    Erode();
    cv::Mat temp = image.clone();
    Dilate();
    image -= temp;
}

void ProcessingImage::Skeletonize() {
    cv::Mat skel(image.size(), CV_8U, cv::Scalar(0));
    cv::Mat eroded;
    do {
        cv::Mat img = image.clone();
        Erode();
        eroded = image.clone();
        Dilate();
        img -= image;

        cv::bitwise_or(skel, img, skel);
        image = eroded;
    } while (!(cv::countNonZero(eroded) == 0));
    image = skel;
}

void ProcessingImage::SetStructuralElement(const StructuralElement &element,
                                           const std::vector<float> &params) {
    structuralElementType = element;
    structuralElementParams = params;
}

void ProcessingImage::SetStructuralElementArgument(cl::Kernel &kernel) {
    switch (structuralElementType) {
    case ELLIPSE: {
        cl_float3 ellipseParams =
            (cl_float3) { { structuralElementParams[0],
                            structuralElementParams[1],
                            structuralElementParams[2] } };
        kernel.setArg(2, ellipseParams);
        break;
    }
    case RECTANGLE:
    case CROSS: {
        cl_int2 rectParams = (cl_int2) { {(int)structuralElementParams[0],
                                          (int)structuralElementParams[1] } };
        kernel.setArg(2, rectParams);
        break;
    }
    default:
        break;
    }
}

void ProcessingImage::Binarize(const unsigned int &minimum,
                               const unsigned int &maximum) {
    cl::Kernel kernel = cl::Kernel(openCLManager->program, "Binarize");

    cl::ImageFormat format(CL_INTENSITY, CL_UNSIGNED_INT8);
    cl::ImageFormat formatOut(CL_INTENSITY, CL_UNSIGNED_INT8);
    cl::Image2D image_in(openCLManager->context,
                         CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                         image.cols, image.rows, 0, image.data);
    cl::Image2D image_out(openCLManager->context, CL_MEM_WRITE_ONLY, formatOut,
                          image.cols, image.rows);
    kernel.setArg(2, minimum);
    kernel.setArg(3, maximum);
    Process(kernel, image_in, image_out);
}

void ProcessingImage::Process(cl::Kernel &kernel, const cl::Image2D &image_in,
                              cl::Image2D &image_out) {
    kernel.setArg(0, image_in);
    kernel.setArg(1, image_out);
    openCLManager->queue.enqueueNDRangeKernel(
        kernel, cl::NullRange, cl::NDRange(image.cols, image.rows), localRange,
        NULL, NULL);
    openCLManager->queue.enqueueReadImage(image_out, CL_TRUE, origin, region, 0,
                                          0, image.data);
}
