#include "Kernels3d.cl"

__kernel void threshold(__read_only image2d_t imageIn,
                        __write_only image2d_t imageOut,
                        const float threshold) {
  float out_color = 1;

  int2 coord = (int2){ get_global_id(0), get_global_id(1) };

  float pixel = read_imagef(imageIn, sampler, coord).x;

  if (pixel > threshold) {
    out_color = 1;
  } else {
    out_color = 0;
  }
  write_imagef(imageOut, coord, out_color);
}
__kernel void Binarize(__read_only image2d_t imageIn,
                       __write_only image2d_t imageOut, const uint min,
                       const uint max) {

  int2 coord = (int2){ get_global_id(0), get_global_id(1) };

  uint4 pixel = read_imageui(imageIn, sampler, coord);
  float out_color = 0;
  if (pixel.s3 > min && pixel.s3 <= max) {
    out_color = 1;
  }
  write_imagef(imageOut, coord, out_color);
}
__constant int squareSize = 12;
__kernel void ErodeEllipse(__read_only image2d_t imageIn,
                           __write_only image2d_t imageOut,
                           const float3 ellipseParams) {
  int2 coord = (int2){ get_global_id(0), get_global_id(1) };
  float sine = sin(ellipseParams.x);
  float cosine = cos(ellipseParams.x);
  float Ri = ellipseParams.y;
  float Rj = ellipseParams.z;

  for (int i = -squareSize; i < squareSize; i++) {
    for (int j = -squareSize; j < squareSize; j++) {
      float m = i * cosine - j * sine;
      float n = i * sine + j * cosine;
      if (m * m + (Ri * Ri / Rj / Rj) * n * n < Ri * Ri) {
        int2 ijCoord = coord + (int2){ i, j };
        float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
        if (inPixel == 0) {
          write_imagef(imageOut, coord, 0);
          return;
        }
      }
    }
  }
  write_imagef(imageOut, coord, 1);
}
__kernel void DilateEllipse(__read_only image2d_t imageIn,
                            __write_only image2d_t imageOut,
                            const float3 ellipseParams) {
  int2 coord = (int2){ get_global_id(0), get_global_id(1) };
  float sine = sin(ellipseParams.x);
  float cosine = cos(ellipseParams.x);
  float Ri = ellipseParams.y;
  float Rj = ellipseParams.z;

  for (int i = -squareSize; i < squareSize; i++) {
    for (int j = -squareSize; j < squareSize; j++) {
      float m = i * cosine - j * sine;
      float n = i * sine + j * cosine;
      if (m * m + (Ri * Ri / Rj / Rj) * n * n < Ri * Ri) {
        int2 ijCoord = coord + (int2){ i, j };
        float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
        if (inPixel == 1) {
          write_imagef(imageOut, coord, 1);
          return;
        }
      }
    }
  }
  write_imagef(imageOut, coord, 0);
}

__kernel void ErodeRectangle(__read_only image2d_t imageIn,
                             __write_only image2d_t imageOut,
                             const int2 rectangle) {
  int2 coord = (int2){ get_global_id(0), get_global_id(1) };
  for (int i = -rectangle.x; i <= rectangle.x; i++) {
    for (int j = -rectangle.y; j <= rectangle.y; j++) {
      int2 ijCoord = coord + (int2){ i, j };
      float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
      if (inPixel == 0) {
        write_imagef(imageOut, coord, 0);
        return;
      }
    }
  }
  write_imagef(imageOut, coord, 1);
}
__kernel void DilateRectangle(__read_only image2d_t imageIn,
                              __write_only image2d_t imageOut,
                              const int2 rectangle) {
  int2 coord = (int2){ get_global_id(0), get_global_id(1) };
  for (int i = -rectangle.x; i <= rectangle.x; i++) {
    for (int j = -rectangle.y; j <= rectangle.y; j++) {
      int2 ijCoord = coord + (int2){ i, j };
      float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
      if (inPixel == 1) {
        write_imagef(imageOut, coord, 1);
        return;
      }
    }
  }
  write_imagef(imageOut, coord, 0);
}

__kernel void ErodeCross(__read_only image2d_t imageIn,
                         __write_only image2d_t imageOut,
                         const int2 rectangle) {
  int2 coord = (int2){ get_global_id(0), get_global_id(1) };
  write_imagef(imageOut, coord, 1);
  for (int i = -rectangle.x; i <= rectangle.x; i++) {
    int hor = 0;
    if (i == 0) {
      hor = rectangle.y;
    }

    for (int j = -hor; j <= hor; j++) {
      if (read_imagef(imageIn, sampler, coord + (int2){ i, j }).x == 0) {
        write_imagef(imageOut, coord, 0);
        return;
      }
    }
  }
}
//   0     1       1 1 1       1
//   1     1 1 0     1     0 1 1
// 1 1 1   1         0         1

__kernel void Skeletonize(__read_only image2d_t imageIn,
                          __write_only image2d_t imageOut,
                          int structElVersion) {
  int2 coord = (int2){ get_global_id(0), get_global_id(1) };
  float outValue = read_imagef(imageIn, sampler, coord).x;
  switch (structElVersion) {
  case 0:
    if ((read_imagef(imageIn, sampler, coord).x) == 1 &&
        (read_imagef(imageIn, sampler, coord + (int2){ -1, -1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 0, -1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 1, -1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 0, 1 }).x == 0))
      outValue = 0;
    break;
  case 1:
    if ((read_imagef(imageIn, sampler, coord).x) == 1 &&
        (read_imagef(imageIn, sampler, coord + (int2){ -1, 1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ -1, 0 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ -1, -1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 1, 0 }).x == 0))
      outValue = 0;
    break;
  case 2:
    if ((read_imagef(imageIn, sampler, coord).x) == 1 &&
        (read_imagef(imageIn, sampler, coord + (int2){ -1, 1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 0, 1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 1, 1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 0, -1 }).x == 0))
      outValue = 0;
    break;
  case 3:
    if ((read_imagef(imageIn, sampler, coord).x) == 1 &&
        (read_imagef(imageIn, sampler, coord + (int2){ 1, -1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 1, 0 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ 1, 1 }).x == 1) &&
        (read_imagef(imageIn, sampler, coord + (int2){ -1, 0 }).x == 0))
      outValue = 0;
    break;
  }
  write_imagef(imageOut, coord, outValue);
}
__kernel void DilateCross(__read_only image2d_t imageIn,
                          __write_only image2d_t imageOut,
                          const int2 rectangle) {
  int2 coord = (int2){ get_global_id(0), get_global_id(1) };
  write_imagef(imageOut, coord, 0);
  for (int i = -rectangle.x; i <= rectangle.x; i++) {
    int hor = 0;
    if (i == 0) {
      hor = rectangle.y;
    }

    for (int j = -hor; j <= hor; j++) {
      if (read_imagef(imageIn, sampler, coord + (int2){ i, j }).x == 1) {
        write_imagef(imageOut, coord, 1);
        return;
      }
    }
  }
}
