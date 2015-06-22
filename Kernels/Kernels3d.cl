constant sampler_t sampler =
    CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE;

#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

void printImage(__write_only image3d_t imageOut, int i, int j, int k,
                int4 image_coord) {
  write_imagef(imageOut, image_coord + (int4){ i, j, k, 0 }, 1);
  write_imagef(imageOut, image_coord + (int4){ i, -j, k, 0 }, 1);
  write_imagef(imageOut, image_coord + (int4){ i, j, -k, 0 }, 1);
  write_imagef(imageOut, image_coord + (int4){ i, -j, -k, 0 }, 1);
  write_imagef(imageOut, image_coord + (int4){ -i, j, k, 0 }, 1);
  write_imagef(imageOut, image_coord + (int4){ -i, -j, k, 0 }, 1);
  write_imagef(imageOut, image_coord + (int4){ -i, j, -k, 0 }, 1);
  write_imagef(imageOut, image_coord + (int4){ -i, -j, -k, 0 }, 1);
}
bool checkImage(int i, int j, int k, int4 coord, __read_only image3d_t image,
                const int value) {
  if (read_imagef(image, sampler, coord + (int4){ i, j, k, 0 }).x == value)
    return true;
  if (read_imagef(image, sampler, coord + (int4){ i, -j, k, 0 }).x == value)
    return true;
  if (read_imagef(image, sampler, coord + (int4){ i, j, -k, 0 }).x == value)
    return true;
  if (read_imagef(image, sampler, coord + (int4){ i, -j, -k, 0 }).x == value)
    return true;
  if (read_imagef(image, sampler, coord + (int4){ -i, j, k, 0 }).x == value)
    return true;
  if (read_imagef(image, sampler, coord + (int4){ -i, -j, k, 0 }).x == value)
    return true;
  if (read_imagef(image, sampler, coord + (int4){ -i, j, -k, 0 }).x == value)
    return true;
  if (read_imagef(image, sampler, coord + (int4){ -i, -j, -k, 0 }).x == value)
    return true;
  return false;
}
__kernel void Erode3dEllipse(__read_only image3d_t imageIn,
                             __write_only image3d_t imageOut,
                             const float3 structParams) {
  int4 coord = (int4){ get_global_id(0), get_global_id(1), get_global_id(2),
                       get_global_id(3) };
  const float c = (float)structParams.z;
  write_imagef(imageOut, coord, 1);
  if (read_imagef(imageIn, sampler, coord + (int4){ 0, 0, (int)-c, 0 }).x ==
          0 ||
      (read_imagef(imageIn, sampler, coord + (int4){ 0, 0, (int)c, 0 }).x ==
       0)) {
    write_imagef(imageOut, coord, 0);
    return;
  }
  for (float k = 0; k <= c; ++k) {
    float xRadius =
        (float)structParams.x * sqrt(1.0 - (c - k) * (c - k) / (c * c));
    float yRadius =
        (float)structParams.y * sqrt(1.0 - (c - k) * (c - k) / (c * c));

    for (float i = 0; i <= (int)xRadius; ++i) {
      int yRadiusNew =
          yRadius *
          sqrt(1.0 - (xRadius - i) * (xRadius - i) / (xRadius * xRadius));
      for (int j = 0; j <= yRadiusNew; ++j) {
        if (checkImage((int)xRadius - i, j, (int)c - k, coord, imageIn, 0)) {
          write_imagef(imageOut, coord, 0);
          return;
        }
      }
    }
  }
}
__kernel void Dilate3dEllipse(__read_only image3d_t imageIn,
                              __write_only image3d_t imageOut,
                              const float3 structParams) {
  int4 coord = (int4){ get_global_id(0), get_global_id(1), get_global_id(2),
                       get_global_id(3) };
  const float c = (float)structParams.z;
  if (read_imagef(imageIn, sampler, coord + (int4){ 0, 0, (int)-c, 0 }).x ==
          1 ||
      (read_imagef(imageIn, sampler, coord + (int4){ 0, 0, (int)c, 0 }).x ==
       1)) {
    write_imagef(imageOut, coord, 1);
    return;
  }
  for (float k = 0; k <= c; ++k) {
    float xRadius =
        (float)structParams.x * sqrt(1.0 - (c - k) * (c - k) / (c * c));
    float yRadius =
        (float)structParams.y * sqrt(1.0 - (c - k) * (c - k) / (c * c));

    for (float i = 0; i <= (int)xRadius; ++i) {
      int yRadiusNew =
          yRadius *
          sqrt(1.0 - (xRadius - i) * (xRadius - i) / (xRadius * xRadius));
      for (int j = 0; j <= yRadiusNew; ++j) {
        if (checkImage((int)xRadius - i, j, (int)c - k, coord, imageIn, 1)) {
          write_imagef(imageOut, coord, 1);
          return;
        }
      }
    }
  }
  write_imagef(imageOut, coord, 0);
}
constant sampler_t sampler2 = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_NONE;

__kernel void Dilate3dEllipseImage(__read_only image3d_t imageIn,
                                   __write_only image3d_t imageOut,
                                   __read_only image3d_t ellipse,
                                   const float3 structParams) {
  int4 coord = (int4){ get_global_id(0), get_global_id(1), get_global_id(2),
                       get_global_id(3) };

  if (coord.x != 40 || coord.y != 40 || coord.z != 40)
    return;
  for (int i = 0; i <= structParams.x * 2; ++i) {
    for (int j = 0; j <= structParams.y * 2; ++j) {
      for (int k = 0; k <= structParams.z * 2; ++k) {
        float inPixel = read_imagef(ellipse, sampler, (int4){ i, j, k, 0 }).x;
        if (inPixel == 1)
          write_imagef(imageOut, coord + (int4){ i, j, k, 0 }, inPixel);
      }
    }
  }
  // write_imagef(imageOut, coord, 0);
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

  write_imagef(imageOut, image_coord, 0);
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
