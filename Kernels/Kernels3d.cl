constant sampler_t sampler =
    CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE;

#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

__kernel void Erode3dEllipse(__read_only image3d_t imageIn,
                             __write_only image3d_t imageOut,
                             const float3 structParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;
  write_imagef(imageOut, image_coord, 0);
}
void printImage(__write_only image3d_t imageOut, int i, int j, int k,
                int4 image_coord) {
  int4 ijCoord = image_coord + (int4){ i, j, k, 0 };
  write_imagef(imageOut, ijCoord, 1);
  ijCoord = image_coord + (int4){ i, -j, k, 0 };
  write_imagef(imageOut, ijCoord, 1);
  ijCoord = image_coord + (int4){ i, j, -k, 0 };
  write_imagef(imageOut, ijCoord, 1);
  ijCoord = image_coord + (int4){ i, -j, -k, 0 };
  write_imagef(imageOut, ijCoord, 1);
  ijCoord = image_coord + (int4){ -i, j, k, 0 };
  write_imagef(imageOut, ijCoord, 1);
  ijCoord = image_coord + (int4){ -i, -j, k, 0 };
  write_imagef(imageOut, ijCoord, 1);
  ijCoord = image_coord + (int4){ -i, j, -k, 0 };
  write_imagef(imageOut, ijCoord, 1);
  ijCoord = image_coord + (int4){ -i, -j, -k, 0 };
  write_imagef(imageOut, ijCoord, 1);
}
__kernel void Dilate3dEllipse(__read_only image3d_t imageIn,
                              __write_only image3d_t imageOut,
                              const float3 structParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  const float c = (float)structParams.z;

  if (image_coord.x != 40 || image_coord.y != 40 || image_coord.z != 40)
    return;
  for (float k = 0; k <= c; ++k) {
    float xRadius =
        (float)structParams.x * sqrt(1.0 - (c - k) * (c - k) / (c * c));
    float yRadius =
        (float)structParams.y * sqrt(1.0 - (c - k) * (c - k) / (c * c));
    if (xRadius == 0) {
      ;
    } else {
      for (float i = 0; i <= (int)xRadius; ++i) {
        float yRadiusNew =
            yRadius *
            sqrt(1.0 - (xRadius - i) * (xRadius - i) / (xRadius * xRadius));
        for (int j = 0; j <= (int)yRadiusNew; ++j) {
          ;
        }
      }
    }
  }
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
