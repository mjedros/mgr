#pragma once

namespace Mgr {
class Image3d;
class CPUSkeletonize {
public:
  CPUSkeletonize() = default;
  void perform(int iter, int subiter, Image3d *image);
};
}
