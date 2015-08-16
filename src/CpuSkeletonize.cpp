#include "CpuSkeletonize.h"
#include "Image3d.h"
#include <thread>
#include <mutex>
namespace Mgr {
static const u_int8_t THREAD_NUMBER = 2;
using values = std::vector<std::vector<int>>;
static std::mutex Mutex;
void checkThread(const int &threadNum, values valueOne, Image3d inImage,
                 values valueZero, Image3d *outImage,
                 values valueAny = values()) {
  int minDepth = threadNum * inImage.getDepth() / THREAD_NUMBER;
  int maxDepth = (1 + threadNum) * inImage.getDepth() / THREAD_NUMBER;
  if (threadNum == 0)
    minDepth += 2;
  if (threadNum == THREAD_NUMBER - 1)
    maxDepth -= 2;
  for (int i = minDepth; i < maxDepth; ++i) {
    for (int j = 2; j < inImage.getRows() - 2; ++j) {
      for (int k = 2; k < inImage.getCols() - 2; ++k) {
        if (inImage.getPoint(k, j, i) == 0)
          continue;
        bool found = false;
        for (auto one : valueOne) {
          if (inImage.getPoint(k + one[0], j + one[1], i + one[2]) == 0) {
            found = true;
            break;
          }
        }
        if (!found) {
          for (auto one : valueZero) {
            if (inImage.getPoint(k + one[0], j + one[1], i + one[2]) != 0) {
              found = true;
              break;
            }
          }
        }
        if (!found && !valueAny.empty()) {
          found = true;
          for (auto any : valueAny) {
            if (inImage.getPoint(k + any[0], j + any[1], i + any[2]) != 0) {
              found = false;
              break;
            }
          }
        }
        if (!found) {
          std::lock_guard<std::mutex> lock(Mutex);
          outImage->setPoint(k, j, i, 0);
        }
      }
    }
  }
}
void check(values valueOne, Image3d inImage, values valueZero,
           Image3d *outImage, values valueAny = values()) {
  std::thread t[THREAD_NUMBER];
  for (int i = 0; i < THREAD_NUMBER; ++i) {
    t[i] = std::thread(checkThread, i, valueOne, inImage, valueZero, outImage,
                       valueAny);
  }

  for (int i = 0; i < THREAD_NUMBER; ++i) {
    t[i].join();
  }
}

void checkA(Image3d *outImage, Image3d inImage, int structElemV) {
  switch (structElemV) {
  case 0: {
    values valueOne = { { 1, 0, 0 } };
    values valueZero = { { -1, 1, 1 },
                         { -1, 1, 0 },
                         { -1, 1, -1 },
                         { -1, 0, 1 },
                         { -1, 0, 0 },
                         { -1, 0, -1 },
                         { -1, -1, 1 },
                         { -1, -1, 0 },
                         { -1, -1, -1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 1: {
    values valueOne = { { -1, 0, 0 }, { -2, 0, 0 } };
    values valueZero = { { 1, 1, 1 },
                         { 1, 1, 0 },
                         { 1, 1, -1 },
                         { 1, 0, 1 },
                         { 1, 0, 0 },
                         { 1, 0, -1 },
                         { 1, -1, 1 },
                         { 1, -1, 0 },
                         { 1, -1, -1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }

  case 2: {
    values valueOne = { { -1, 0, 0 }, { -2, 0, 0 }, { 0, 1, 0 } };
    values valueZero = { { 0, -1, 1 },
                         { 0, -1, 0 },
                         { 0, -1, -1 },
                         { 1, -1, 1 },
                         { 1, -1, 0 },
                         { 1, -1, -1 },
                         { 1, 0, -1 },
                         { 1, 0, 1 },
                         { 1, 0, 1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }

  case 3: {
    values valueOne = { { 0, 1, 0 }, { 1, 0, 0 } };
    values valueZero = { { 0, -1, 1 },
                         { 0, -1, 0 },
                         { 0, -1, -1 },
                         { -1, -1, 1 },
                         { -1, -1, 0 },
                         { -1, -1, -1 },
                         { -1, 0, -1 },
                         { -1, 0, 1 },
                         { -1, 0, 1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }

  case 4: {
    values valueOne = { { 0, 0, 1 }, { 0, -1, 0 }, { 0, -2, 0 } };
    values valueZero = { { 1, 0, -1 },
                         { 0, 0, -1 },
                         { -1, 0, -1 },
                         { -1, 1, -1 },
                         { 0, 1, -1 },
                         { 1, 1, -1 },
                         { -1, 1, 0 },
                         { 0, 1, 0 },
                         { 1, 1, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }

  case 5: {
    values valueOne = { { 0, 1, 0 }, { 0, 0, 1 } };
    values valueZero = { { 1, 0, -1 },
                         { 0, 0, -1 },
                         { -1, 0, -1 },
                         { -1, -1, -1 },
                         { 0, -1, -1 },
                         { 1, -1, -1 },
                         { -1, -1, 0 },
                         { 0, -1, 0 },
                         { 1, -1, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  }
}
void checkB(Image3d *outImage, Image3d inImage, int structElVersion) {
  switch (structElVersion) {
  case 0: {
    values valueOne = {
      { 1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, -2, 0, 0 },
    };
    values valueZero = { { -1, 1, 1, 0 },
                         { -1, 1, 0, 0 },
                         { -1, 1, -1, 0 },
                         { 0, 1, 1, 0 },
                         { 0, 1, 0, 0 },
                         { 0, 1, -1, 0 },
                         { -1, 0, -1, 0 },
                         { -1, 0, 1, 0 },
                         { -1, 0, 1, 0 } };
    check(valueOne, inImage, valueZero, outImage);

    break;
  }
  case 1: {
    values valueOne = {
      { -1, 0, 0, 0 }, { -2, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, -2, 0, 0 }
    };
    values valueZero = { { 0, 1, 1, 0 },
                         { 0, 1, 0, 0 },
                         { 0, 1, -1, 0 },
                         { 1, 1, 1, 0 },
                         { 1, 1, 0, 0 },
                         { 1, 1, -1, 0 },
                         { 1, 0, -1, 0 },
                         { 1, 0, 1, 0 },
                         { 1, 0, 1, 0 } };

    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 2: {
    values valueOne = { { -1, 0, 0, 0 }, { -2, 0, 0, 0 }, { 0, 1, 0, 0 } };
    values valueZero = { { 0, -1, 1, 0 },
                         { 0, -1, 0, 0 },
                         { 0, -1, -1, 0 },
                         { 1, -1, 1, 0 },
                         { 1, -1, 0, 0 },
                         { 1, -1, -1, 0 },
                         { 1, 0, -1, 0 },
                         { 1, 0, 1, 0 },
                         { 1, 0, 1, 0 } };
    check(valueOne, inImage, valueZero, outImage);

    break;
  }
  case 3: {
    values valueOne = { { 0, 1, 0, 0 }, { 1, 0, 0, 0 } };
    values valueZero = { { 0, -1, 1, 0 },
                         { 0, -1, 0, 0 },
                         { 0, -1, -1, 0 },
                         { -1, -1, 1, 0 },
                         { -1, -1, 0, 0 },
                         { -1, -1, -1, 0 },
                         { -1, 0, -1, 0 },
                         { -1, 0, 1, 0 },
                         { -1, 0, 1, 0 } };

    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 4: {
    values valueOne = { { 0, 0, 1, 0 }, { 0, -1, 0, 0 }, { 0, -2, 0, 0 } };
    values valueZero = { { 1, 0, -1, 0 },
                         { 0, 0, -1, 0 },
                         { -1, 0, -1, 0 },
                         { -1, 1, -1, 0 },
                         { 0, 1, -1, 0 },
                         { 1, 1, -1, 0 },
                         { -1, 1, 0, 0 },
                         { 0, 1, 0, 0 },
                         { 1, 1, 0, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 5: {
    values valueOne = { { 0, 1, 0, 0 }, { 0, 0, 1, 0 } };
    values valueZero = { { 1, 0, -1, 0 },
                         { 0, 0, -1, 0 },
                         { -1, 0, -1, 0 },
                         { -1, -1, -1, 0 },
                         { 0, -1, -1, 0 },
                         { 1, -1, -1, 0 },
                         { -1, -1, 0, 0 },
                         { 0, -1, 0, 0 },
                         { 1, -1, 0, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }

  case 6: {
    values valueOne = {
      { 0, -1, 0, 0 }, { 0, -2, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 }
    };
    values valueZero = { { 1, 0, 1, 0 },
                         { 0, 0, 1, 0 },
                         { -1, 0, 1, 0 },
                         { -1, 1, 1, 0 },
                         { 0, 1, 1, 0 },
                         { 1, 1, 1, 0 },
                         { -1, 1, 0, 0 },
                         { 0, 1, 0, 0 },
                         { 1, 1, 0, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 7: {
    values valueOne = { { 0, 1, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 } };
    values valueZero = { { 1, 0, 1, 0 },
                         { 0, 0, 1, 0 },
                         { -1, 0, 1, 0 },
                         { -1, -1, 1, 0 },
                         { 0, -1, 1, 0 },
                         { 1, -1, 1, 0 },
                         { -1, -1, 0, 0 },
                         { 0, -1, 0, 0 },
                         { 1, -1, 0, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 8: {
    values valueOne = { { 0, -1, 0, 0 }, { 0, -2, 0, 0 }, { 0, 0, 1, 0 } };
    values valueZero = { { 1, 1, 0, 0 },
                         { 1, 0, 0, 0 },
                         { 1, -1, 0, 0 },
                         { 0, 1, -1, 0 },
                         { 0, 0, -1, 0 },
                         { 0, -1, -1, 0 },
                         { 1, 1, -1, 0 },
                         { 1, 0, -1, 0 },
                         { 1, -1, -1, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 9: {
    values valueOne = { { 0, 0, 1, 0 }, { 1, 0, 0, 0 } };
    values valueZero = { { -1, 1, 0, 0 },
                         { -1, 0, 0, 0 },
                         { -1, -1, 0, 0 },
                         { 0, 1, -1, 0 },
                         { 0, 0, -1, 0 },
                         { 0, -1, -1, 0 },
                         { -1, 1, -1, 0 },
                         { -1, 0, -1, 0 },
                         { -1, -1, -1, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 10: {
    values valueOne = {
      { -1, 0, 0, 0 }, { -2, 0, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 }
    };
    values valueZero = { { 1, 1, 1, 0 },
                         { 1, 0, 1, 0 },
                         { 1, -1, 1, 0 },
                         { 0, 1, 1, 0 },
                         { 0, 0, 1, 0 },
                         { 0, -1, 1, 0 },
                         { 1, 1, 0, 0 },
                         { 1, 0, 0, 0 },
                         { 1, -1, 0, 0 } };

    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 11: {
    values valueOne = { { 1, 0, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 } };
    values valueZero = { { -1, 1, 1, 0 },
                         { -1, 0, 1, 0 },
                         { -1, -1, 1, 0 },
                         { 0, 1, 1, 0 },
                         { 0, 0, 1, 0 },
                         { 0, -1, 1, 0 },
                         { -1, 1, 0, 0 },
                         { -1, 0, 0, 0 },
                         { -1, -1, 0, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  }
}
void checkC(Image3d *outImage, Image3d inImage, int structElVersion) {
  switch (structElVersion) {
  case 0: {
    values valueOne = { { 0, 0, 1 }, { 0, 1, 0 }, { -1, 0, 0 }, { -2, 0, 0 } };

    values valueZero = { { 1, 0, 0 },
                         { 0, -1, 0 },
                         { 1, -1, 0 },

                         { 0, 0, -1 },
                         { 0, -1, -1 },
                         { 1, -1, -1 },
                         { 1, 0, -1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 1: {
    values valueOne = { { 0, 0, 1 }, { 0, 1, 0 }, { 1, 0, 0 } };

    values valueZero = { { -1, 0, 0 },
                         { 0, -1, 0 },
                         { -1, -1, 0 },

                         { 0, 0, -1 },
                         { 0, -1, -1 },
                         { -1, -1, -1 },
                         { -1, 0, -1 } };

    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 2: {
    values valueOne = { { 0, 0, 1 }, { 1, 0, 0 }, { 0, -1, 0 }, { 0, -2, 0 } };

    values valueZero = { { 0, 1, 0 },
                         { -1, 1, 0 },
                         { -1, 0, 0 },

                         { 0, 0, -1 },
                         { 0, 1, -1 },
                         { -1, 1, -1 },
                         { -1, 0, -1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }

  case 3: {
    values valueOne = {
      { -1, 0, 0 }, { -2, 0, 0 }, { 0, -1, 0 }, { 0, -2, 0 }, { 0, 0, 1 }
    };

    values valueZero = { { 0, 1, 0 },
                         { 1, 1, 0 },
                         { 1, 0, 0 },
                         { 0, 0, -1 },
                         { 0, 1, -1 },
                         { 1, 1, -1 },
                         { 1, 0, -1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 4: {
    values valueOne = {
      { -1, 0, 0 }, { -2, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 }, { 0, 0, -2 }
    };

    values valueZero = { { 0, -1, 0 },
                         { 1, -1, 0 },
                         { 1, 0, 0 },
                         { 0, 0, 1 },
                         { 0, -1, 1 },
                         { 1, -1, 1 },
                         { 1, 0, 1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 5: {
    values valueOne = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, -1 }, { 0, 0, -2 } };

    values valueZero = { { 0, -1, 0 },
                         { -1, -1, 0 },
                         { -1, 0, 0 },
                         { 0, 0, 1 },
                         { 0, -1, 1 },
                         { -1, -1, 1 },
                         { -1, 0, 1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 6: {
    values valueOne = {
      { 1, 0, 0 }, { 0, -1, 0 }, { 0, -2, 0 }, { 0, 0, -1 }, { 0, 0, -2 }
    };

    values valueZero = { { 0, 1, 0 },
                         { -1, 1, 0 },
                         { -1, 0, 0 },
                         { 0, 0, 1 },
                         { 0, 1, 1 },
                         { -1, 1, 1 },
                         { -1, 0, 1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 7: {
    values valueOne = { { -1, 0, 0 },
                        { -2, 0, 0 },
                        { 0, -1, 0 },
                        { 0, -2, 0 },
                        { 0, 0, -1 },
                        { 0, 0, -2 } };

    values valueZero = { { 0, 1, 0 },
                         { 1, 1, 0 },
                         { 1, 0, 0 },
                         { 0, 0, 1 },
                         { 0, 1, 1 },
                         { 1, 1, 1 },
                         { 1, 0, 1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  }
}
void checkD(Image3d *outImage, Image3d inImage, int structElVersion) {
  switch (structElVersion) {
  case 0: {
    values valueOne = { { 0, -1, 1 } };
    values valueAny = {
      { 0, -2, 0 }, { 0, -2, 1 }, { 0, -2, 2 }, { 0, -1, 2 }, { 0, 0, 2 }
    };
    values valueZero = { { 0, 0, -1 },
                         { 1, 0, -1 },
                         { 1, 1, -1 },
                         { 0, 1, -1 },
                         { -1, 0, -1 },
                         { -1, -1, -1 },
                         { 0, -1, -1 },
                         { -1, 1, -1 },
                         { 1, -1, -1 },

                         { 0, -1, 0 },
                         { -1, 1, 0 },
                         { 0, 1, 0 },
                         { 1, 1, 0 },

                         { 0, 0, 1 },
                         { 0, 1, 1 },
                         { 1, 1, 1 },
                         { -1, 1, 1 } };
    check(valueOne, inImage, valueZero, outImage, valueAny);
    break;
  }
  case 1: {
    values valueOne = { { 0, -1, -1 } };
    values valueAny = {
      { 0, -2, 0 }, { 0, -2, -1 }, { 0, -2, -2 }, { 0, -1, -2 }, { 0, 0, -2 }
    };
    values valueZero = { { 0, 0, 1 },
                         { 1, 0, 1 },
                         { 1, 1, 1 },
                         { 0, 1, 1 },
                         { -1, 0, 1 },
                         { -1, -1, 1 },
                         { 0, -1, 1 },
                         { -1, 1, 1 },
                         { 1, -1, 1 },

                         { -1, 1, 0 },
                         { 0, 1, 0 },
                         { 1, 1, 0 },
                         { 0, -1, 0 },

                         { 0, 0, -1 },
                         { 0, 1, -1 },
                         { 1, 1, -1 },
                         { -1, 1, -1 } };

    break;
  }
  case 2: {
    values valueOne = { { -1, -1, 0 } };
    values valueAny = {
      { -2, 0, 0 }, { -2, -1, 0 }, { -2, -2, 0 }, { -1, -2, 0 }, { 0, -2, 0 }
    };
    values valueZero = { { 1, 1, 0 },
                         { 0, 1, 0 },
                         { -1, 1, 0 },
                         { 1, 1, -1 },
                         { 0, 1, -1 },
                         { -1, 1, -1 },
                         { 1, 1, 1 },
                         { 0, 1, 1 },
                         { -1, 1, 1 },

                         { 1, -1, -1 },
                         { 1, 0, -1 },
                         { 1, 0, 0 },
                         { 1, -1, 0 },
                         { 1, 0, 1 },

                         { -1, 0, 0 },
                         { 0, -1, 0 } };
    check(valueOne, inImage, valueZero, outImage, valueAny);
    break;
  }
  case 3: {
    values valueOne = { { 1, 1, 0 } };
    values valueAny = {
      { 2, 0, 0 }, { 2, -1, 0 }, { 2, -2, 0 }, { 1, -2, 0 }, { 0, -2, 0 }
    };
    values valueZero = { { 1, 1, 0 },
                         { 0, 1, 0 },
                         { -1, 1, 0 },
                         { 1, 1, -1 },
                         { 0, 1, -1 },
                         { -1, 1, -1 },
                         { 1, 1, 1 },
                         { 0, 1, 1 },
                         { -1, 1, 1 },

                         { -1, 0, 1 },
                         { -1, 0, -1 },
                         { -1, 0, 0 },
                         { -1, -1, 0 },
                         { -1, -1, -1 },
                         { -1, -1, 1 },

                         { 1, 0, 0 },
                         { 0, -1, 0 } };
    check(valueOne, inImage, valueZero, outImage, valueAny);
    break;
  }
  case 4: {
    values valueOne = { { -1, 0, 1 } };
    values valueAny = {
      { 0, 0, 2 }, { -1, 0, 2 }, { -2, 0, 2 }, { -2, 0, 1 }, { -2, 0, 0 }
    };
    values valueZero = { { 1, 0, 0 },
                         { 1, 0, 1 },
                         { 1, 0, -1 },
                         { 1, 1, 0 },
                         { 1, 1, 1 },
                         { 1, 1, -1 },
                         { 1, -1, 0 },
                         { 1, -1, 1 },
                         { 1, -1, -1 },

                         { -1, 0, -1 },
                         { -1, 1, -1 },
                         { -1, -1, -1 },
                         { 0, 0, -1 },
                         { 0, 1, -1 },
                         { 0, -1, -1 },

                         { 0, 0, 1 },
                         { -1, 0, 0 } };
    check(valueOne, inImage, valueZero, outImage, valueAny);
    break;
  }
  case 5: {
    values valueOne = { { -1, 0, -1 } };
    values valueAny = {
      { 0, 0, -2 }, { -1, 0, -2 }, { -2, 0, -2 }, { -2, 0, -1 }, { -2, 0, 0 }
    };
    values valueZero = { { 1, 0, 0 },
                         { 1, 0, 1 },
                         { 1, 0, -1 },
                         { 1, 1, 0 },
                         { 1, 1, 1 },
                         { 1, 1, -1 },
                         { 1, -1, 0 },
                         { 1, -1, 1 },
                         { 1, -1, -1 },

                         { -1, 0, 1 },
                         { -1, 1, 1 },
                         { -1, -1, 1 },
                         { 0, 0, 1 },
                         { 0, 1, 1 },
                         { 0, -1, 1 },

                         { 0, 0, -1 },
                         { -1, 0, 0 } };
    check(valueOne, inImage, valueZero, outImage, valueAny);
    break;
  }
  case 6: {
    values valueOne = { { 0, 0, -1 }, { 0, 0, 1 }, { 1, 1, 0 } };
    values valueZero = { { 0, -1, 0 },
                         { 1, -1, 0 },
                         { -1, -1, 0 },
                         { -1, 0, 0 },
                         { 1, 0, 0 },
                         { -1, 1, 0 },
                         { 0, 1, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 7: {
    values valueOne = { { 0, 0, -1 }, { 0, 0, 1 }, { -1, 1, 0 } };
    values valueZero = { { 0, -1, 0 },
                         { 1, -1, 0 },
                         { -1, -1, 0 },
                         { -1, 0, 0 },
                         { 1, 0, 0 },
                         { 1, 1, 0 },
                         { 0, 1, 0 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 8: {
    values valueOne = { { -1, 0, 0 }, { 1, 0, 0 }, { 0, 1, 1 } };
    values valueZero = { { 0, 0, -1 },
                         { 0, -1, -1 },
                         { 0, 1, -1 },
                         { 0, -1, 0 },
                         { 0, 1, 0 },
                         { 0, -1, 1 },
                         { 0, 0, 1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 9: {
    values valueOne = { { -1, 0, 0 }, { 1, 0, 0 }, { 0, 1, -1 } };
    values valueZero = { { 0, 0, -1 },
                         { 0, -1, -1 },
                         { 0, -1, 0 },
                         { 0, 1, 0 },
                         { 0, 1, 1 },
                         { 0, -1, 1 },
                         { 0, 0, 1 } };

    break;
  }
  case 10: {
    values valueOne = { { 0, 1, 0 }, { 0, 1, 0 }, { 1, 0, -1 } };
    values valueZero = { { 0, 0, 1 },
                         { 1, 0, 1 },
                         { -1, 0, 1 },
                         { 1, 0, 0 },
                         { -1, 0, 0 },
                         { -1, 0, -1 },
                         { 0, 0, -1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  case 11: {
    values valueOne = { { 0, 1, 0 }, { 0, 1, 0 }, { 1, 0, 1 } };
    values valueZero = { { 0, 0, -1 },
                         { 1, 0, -1 },
                         { -1, 0, -1 },
                         { 1, 0, 0 },
                         { -1, 0, 0 },
                         { -1, 0, 1 } };
    check(valueOne, inImage, valueZero, outImage);
    break;
  }
  }
}
void CPUSkeletonize::perform(int type, int structElemV, Image3d *image) {
  Image3d imageIn(image->getRows(), image->getCols(),
                  image->get3dMatImage().clone());
  switch (type) {
  case 0: {
    checkA(image, imageIn, structElemV);
    break;
  }
  case 1: {
    checkB(image, imageIn, structElemV);
    break;
  }
  case 2: {
    checkC(image, imageIn, structElemV);
    break;
  }
  case 3: {
    checkD(image, imageIn, structElemV);
    break;
  }
  }
  imageIn.clear();
}
}
