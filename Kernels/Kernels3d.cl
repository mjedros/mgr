constant sampler_t sampler =
    CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE;

#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

__kernel void Erode3dEllipse(__read_only image3d_t imageIn,
                             __write_only image3d_t imageOut,
                             const int3 structParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;
  write_imagef(imageOut, image_coord, inPixel);
}
__kernel void Dilate3dEllipse(__read_only image3d_t imageIn,
                              __write_only image3d_t imageOut,
                              const int3 structParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;

  write_imagef(imageOut, image_coord, inPixel);
}

__kernel void Erode3dRectangle(__read_only image3d_t imageIn,
                               __write_only image3d_t imageOut,
                               const int3 structParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  for (int i = -structParams.x; i <= structParams.x; ++i) {
    for (int j = -structParams.y; j <= structParams.y; ++j) {
      for (int k = -structParams.z; k <= structParams.z; ++k) {
        int4 ijCoord = image_coord + (int4){ i, j, k, 0 };
        float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
        if (inPixel == 0) {
          write_imagef(imageOut, image_coord, 0);
          return;
        }
      }
    }
  }
  write_imagef(imageOut, image_coord, 1);
}
__kernel void Dilate3dRectangle(__read_only image3d_t imageIn,
                                __write_only image3d_t imageOut,
                                const int3 structParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  for (int i = -structParams.x; i <= structParams.x; i++) {
    for (int j = -structParams.y; j <= structParams.y; j++) {
      for (int k = -structParams.z; k <= structParams.z; k++) {
        int4 ijCoord = image_coord + (int4){ i, j, k, 0 };
        float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
        if (inPixel == 1) {
          write_imagef(imageOut, image_coord, 1);
          return;
        }
      }
    }
  }
  write_imagef(imageOut, image_coord, 0);
}

__kernel void Erode3dCross(__read_only image3d_t imageIn,
                           __write_only image3d_t imageOut,
                           const int3 structParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;
  write_imagef(imageOut, image_coord, inPixel);
}

__kernel void Dilate3dCross(__read_only image3d_t imageIn,
                            __write_only image3d_t imageOut,
                            const int3 structParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  write_imagef(imageOut, image_coord, 0);
  for (int k = -structParams.z; k <= structParams.z; ++k) {
    for (int i = -structParams.x; i <= structParams.x; ++i) {
      int hor = (i == 0 ? hor = structParams.y : 0);
      for (int j = -hor; j <= hor; ++j) {
        if (read_imagef(imageIn, sampler, image_coord + (int4){ i, j, k, 0 })
                .x == 1) {
          write_imagef(imageOut, image_coord, 1);
          return;
        }
      }
    }
  }
}
