#include "GUI/VTKData.h"
#include "Logger.h"
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkContourFilter.h>
#include <vtkCellArray.h>

#include <thread>
#include <mutex>
static Mgr::Logger &logger = Mgr::Logger::getInstance();
static const u_int8_t THREAD_NUMBER = 3;
VTKData::VTKData() : renderer(vtkRenderer::New()) {
  renderer->SetBackground(0, 0, 0);
}

VTKData::~VTKData() {
  auto actors = renderer->GetActors();
  auto prevActor = actors ? actors->GetLastActor() : nullptr;
  while (prevActor != nullptr) {
    renderer->RemoveActor(prevActor);
    dataMemHolder.erase(std::remove_if(
        dataMemHolder.begin(), dataMemHolder.end(), [prevActor](auto &data) {
          return data.mapper == prevActor->GetMapper();
        }));
    prevActor = actors->GetLastActor();
  }
}
static std::mutex Mutex;

void VTKData::insertPoints(const int &threadNum,
                           const std::shared_ptr<Mgr::Image3d> &image3d,
                           vtkCellArray *vertices, vtkPoints *points) {
  for (int depth = threadNum * image3d->getDepth() / THREAD_NUMBER;
       depth < (1 + threadNum) * image3d->getDepth() / THREAD_NUMBER; ++depth) {
    const cv::Mat image = image3d->getImageAtDepth(depth);
    for (int col = 0; col < image3d->getCols(); ++col) {
      for (int row = 0; row < image3d->getRows(); ++row) {
        if (image.at<uchar>(row, col) == 255) {
          std::lock_guard<std::mutex> lock(Mutex);
          vtkIdType pid[1];
          pid[0] = points->InsertNextPoint(col, row, depth);
          vertices->InsertNextCell(1, pid);
        }
      }
    }
  }
}

vtkActor *
VTKData::createActorOutOf3dImage(std::tuple<double, double, double> colors) {
  vtkPolyData *polyData = vtkPolyData::New();
  vtkPoints *points = vtkPoints::New();
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  vtkActor *actor = vtkActor::New();
  vtkCellArray *vertices = vtkCellArray::New();
  logger.printFancyLine("Inserting 3d points");
  std::thread t[THREAD_NUMBER];
  for (int i = 0; i < THREAD_NUMBER; ++i) {
    t[i] = std::thread(&VTKData::insertPoints, i, image3d, vertices, points);
  }

  for (int i = 0; i < THREAD_NUMBER; ++i) {
    t[i].join();
  }
  polyData->SetPoints(points);
  polyData->SetVerts(vertices);
  mapper->SetInputData(polyData);
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(std::get<0>(colors), std::get<1>(colors),
                                 std::get<2>(colors));
  dataMemHolder.push_back({ vertices, mapper, points });
  return actor;
}
void VTKData::initVTKImage() {
  std::lock_guard<std::mutex> lock(rendererMutex);

  auto actors = renderer->GetActors();
  auto prevActor = actors->GetLastActor();
  while (prevActor != nullptr) {
    renderer->RemoveActor(prevActor);
    dataMemHolder.erase(std::remove_if(
        dataMemHolder.begin(), dataMemHolder.end(), [prevActor](auto &data) {
          return data.mapper == prevActor->GetMapper();
        }));
    prevActor = actors->GetLastActor();
  }
  vtkActor *actor = createActorOutOf3dImage(std::make_tuple(.9, .9, .9));
  renderer->AddActor(actor);
}

VTKData::dataMem::~dataMem() {
  vertices->Delete();
  mapper->Delete();
  points->Delete();
}
