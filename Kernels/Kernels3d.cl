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

  if (read_imagef(imageIn, sampler, coord + (int4){ 0, 0, (int)c, 0 }).x == 0 ||
      read_imagef(imageIn, sampler, coord + (int4){ 0, 0, (int)-c, 0 }).x ==
          0) {
    write_imagef(imageOut, coord, 0);
    return;
  }

  for (float k = 0; k <= c; ++k) {
    float xRadius =
        (float)structParams.x * sqrt(1.0 - (c - k) * (c - k) / (c * c));
    float yRadius =
        (float)structParams.y * sqrt(1.0 - (c - k) * (c - k) / (c * c));

    for (int i = -xRadius; i <= 0; ++i) {
      for (int j = -yRadius; j <= 0; ++j) {
        if (((i * i) / (xRadius * xRadius) + (j * j) / (yRadius * yRadius) <=
             1))
          if (checkImage(i, j, (int)c - k, coord, imageIn, 0)) {
            write_imagef(imageOut, coord, 0);
            return;
          }
      }
    }
  }
  write_imagef(imageOut, coord, 1);
}
__kernel void Dilate3dEllipse(__read_only image3d_t imageIn,
                              __write_only image3d_t imageOut,
                              const float3 structParams) {
  int4 coord = (int4){ get_global_id(0), get_global_id(1), get_global_id(2),
                       get_global_id(3) };
  const float c = (float)structParams.z;

  if (read_imagef(imageIn, sampler, coord + (int4){ 0, 0, (int)c, 0 }).x == 1 ||
      read_imagef(imageIn, sampler, coord + (int4){ 0, 0, (int)-c, 0 }).x ==
          1) {
    write_imagef(imageOut, coord, 1);
    return;
  }

  for (float k = 0; k <= c; ++k) {
    float xRadius =
        (float)structParams.x * sqrt(1.0 - (c - k) * (c - k) / (c * c));
    float yRadius =
        (float)structParams.y * sqrt(1.0 - (c - k) * (c - k) / (c * c));

    for (int i = -xRadius; i <= 0; ++i) {
      for (int j = -yRadius; j <= 0; ++j) {
        if (((i * i) / (xRadius * xRadius) + (j * j) / (yRadius * yRadius) <=
             1))
          if (checkImage(i, j, (int)c - k, coord, imageIn, 1)) {
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
  write_imagef(imageOut, coord, 0);
  for (int i = 0; i <= structParams.x * 2; ++i) {
    for (int j = 0; j <= structParams.y * 2; ++j) {
      for (int k = 0; k <= structParams.z * 2; ++k) {
        if (read_imagef(ellipse, sampler, (int4){ i, j, k, 0 }).x == 1) {
          if (read_imagef(imageIn, sampler,
                          coord + (int4){ i - structParams.x,
                                          j - structParams.y,
                                          k - structParams.z, 0 }).x == 1) {

            write_imagef(imageOut, coord, 1);
            return;
          }
        }
      }
    }
  }
}
__kernel void Erode3dEllipseImage(__read_only image3d_t imageIn,
                                  __write_only image3d_t imageOut,
                                  __read_only image3d_t ellipse,
                                  const float3 structParams) {
  int4 coord = (int4){ get_global_id(0), get_global_id(1), get_global_id(2),
                       get_global_id(3) };
  write_imagef(imageOut, coord, 1);
  for (int i = 0; i <= structParams.x * 2; ++i) {
    for (int j = 0; j <= structParams.y * 2; ++j) {
      for (int k = 0; k <= structParams.z * 2; ++k) {
        if (read_imagef(ellipse, sampler, (int4){ i, j, k, 0 }).x == 1) {
          if (read_imagef(imageIn, sampler,
                          coord + (int4){ i - structParams.x,
                                          j - structParams.y,
                                          k - structParams.z, 0 }).x == 0) {

            write_imagef(imageOut, coord, 0);
            return;
          }
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
bool checkOne(__read_only image3d_t imageIn, int4 coord, int4 val[], int size) {
  for (int i = 0; i < size; ++i) {
    if (read_imagef(imageIn, sampler, coord + val[i]).x != 1)
      return false;
  }
  return true;
}
bool checkZero(__read_only image3d_t imageIn, int4 coord, int4 val[],
               int size) {
  for (int i = 0; i < size; ++i) {
    if (read_imagef(imageIn, sampler, coord + val[i]).x != 0)
      return false;
  }
  return true;
}
bool checkAny(__read_only image3d_t imageIn, int4 coord, int4 val[], int size) {
  for (int i = 0; i < size; ++i) {
    if (read_imagef(imageIn, sampler, coord + val[i]).x == 1)
      return true;
  }
  return true;
}
bool checkA(__read_only image3d_t imageIn, int4 coord, int structElVersion) {
  bool result = false;
  switch (structElVersion) {
  case 0: {
    int4 valueOne[1] = { { 1, 0, 0, 0 } };
    int4 valueZero[9] = { { -1, 1, 1, 0 },
                          { -1, 1, 0, 0 },
                          { -1, 1, -1, 0 },
                          { -1, 0, 1, 0 },
                          { -1, 0, 0, 0 },
                          { -1, 0, -1, 0 },
                          { -1, -1, 1, 0 },
                          { -1, -1, 0, 0 },
                          { -1, -1, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 1: {
    int4 valueOne[2] = { { -1, 0, 0, 0 }, { -2, 0, 0, 0 } };
    int4 valueZero[9] = { { 1, 1, 1, 0 },
                          { 1, 1, 0, 0 },
                          { 1, 1, -1, 0 },
                          { 1, 0, 1, 0 },
                          { 1, 0, 0, 0 },
                          { 1, 0, -1, 0 },
                          { 1, -1, 1, 0 },
                          { 1, -1, 0, 0 },
                          { 1, -1, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 2) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 2: {
    int4 valueOne[1] = { { 0, 1, 0, 0 } };
    int4 valueZero[9] = { { 1, -1, 1, 0 },
                          { 1, -1, 0, 0 },
                          { 1, -1, -1, 0 },
                          { 0, -1, 1, 0 },
                          { 0, -1, 0, 0 },
                          { 0, -1, -1, 0 },
                          { -1, -1, 1, 0 },
                          { -1, -1, 0, 0 },
                          { -1, -1, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 3: {
    int4 valueOne[2] = { { 0, -1, 0, 0 }, { 0, -2, 0, 0 } };
    int4 valueZero[9] = { { 1, 1, 1, 0 },
                          { 1, 1, 0, 0 },
                          { 1, 1, -1, 0 },
                          { 0, 1, 1, 0 },
                          { 0, 1, 0, 0 },
                          { 0, 1, -1, 0 },
                          { -1, 1, 1, 0 },
                          { -1, 1, 0, 0 },
                          { -1, 1, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 2) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 4: {
    int4 valueOne[1] = { { 0, 0, 1, 0 } };
    int4 valueZero[9] = { { 0, -1, -1, 0 },
                          { 0, 0, -1, 0 },
                          { 0, 1, -1, 0 },
                          { -1, 0, -1, 0 },
                          { -1, 1, -1, 0 },
                          { -1, -1, -1, 0 },
                          { 1, 0, -1, 0 },
                          { 1, 1, -1, 0 },
                          { 1, -1, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 5: {
    int4 valueOne[2] = { { 0, 0, -1, 0 }, { 0, 0, -2, 0 } };
    int4 valueZero[9] = { { 0, -1, 1, 0 },
                          { 0, 0, 1, 0 },
                          { 0, 1, 1, 0 },
                          { -1, 0, 1, 0 },
                          { -1, 1, 1, 0 },
                          { -1, -1, 1, 0 },
                          { 1, 0, 1, 0 },
                          { 1, 1, 1, 0 },
                          { 1, -1, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 2) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  }

  return result;
}
bool checkB(__read_only image3d_t imageIn, int4 coord, int structElVersion) {
  bool result = false;
  switch (structElVersion) {
  case 0: {
    int4 valueOne[3] = {
      { 1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, -2, 0, 0 },
    };
    int4 valueZero[9] = { { -1, 1, 1, 0 },
                          { -1, 1, 0, 0 },
                          { -1, 1, -1, 0 },
                          { 0, 1, 1, 0 },
                          { 0, 1, 0, 0 },
                          { 0, 1, -1, 0 },
                          { -1, 0, -1, 0 },
                          { -1, 0, 1, 0 },
                          { -1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 1: {
    int4 valueOne[4] = {
      { -1, 0, 0, 0 }, { -2, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, -2, 0, 0 }
    };
    int4 valueZero[9] = { { 0, 1, 1, 0 },
                          { 0, 1, 0, 0 },
                          { 0, 1, -1, 0 },
                          { 1, 1, 1, 0 },
                          { 1, 1, 0, 0 },
                          { 1, 1, -1, 0 },
                          { 1, 0, -1, 0 },
                          { 1, 0, 1, 0 },
                          { 1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 4) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 2: {
    int4 valueOne[3] = { { -1, 0, 0, 0 }, { -2, 0, 0, 0 }, { 0, 1, 0, 0 } };
    int4 valueZero[9] = { { 0, -1, 1, 0 },
                          { 0, -1, 0, 0 },
                          { 0, -1, -1, 0 },
                          { 1, -1, 1, 0 },
                          { 1, -1, 0, 0 },
                          { 1, -1, -1, 0 },
                          { 1, 0, -1, 0 },
                          { 1, 0, 1, 0 },
                          { 1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 3: {
    int4 valueOne[2] = { { 0, 1, 0, 0 }, { 1, 0, 0, 0 } };
    int4 valueZero[9] = { { 0, -1, 1, 0 },
                          { 0, -1, 0, 0 },
                          { 0, -1, -1, 0 },
                          { -1, -1, 1, 0 },
                          { -1, -1, 0, 0 },
                          { -1, -1, -1, 0 },
                          { -1, 0, -1, 0 },
                          { -1, 0, 1, 0 },
                          { -1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 2) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 4: {
    int4 valueOne[3] = { { 0, 0, 1, 0 }, { 0, -1, 0, 0 }, { 0, -2, 0, 0 } };
    int4 valueZero[9] = { { 1, 0, -1, 0 },
                          { 0, 0, -1, 0 },
                          { -1, 0, -1, 0 },
                          { -1, 1, -1, 0 },
                          { 0, 1, -1, 0 },
                          { 1, 1, -1, 0 },
                          { -1, 1, 0, 0 },
                          { 0, 1, 0, 0 },
                          { 1, 1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 5: {
    int4 valueOne[2] = { { 0, 1, 0, 0 }, { 0, 0, 1, 0 } };
    int4 valueZero[9] = { { 1, 0, -1, 0 },
                          { 0, 0, -1, 0 },
                          { -1, 0, -1, 0 },
                          { -1, -1, -1, 0 },
                          { 0, -1, -1, 0 },
                          { 1, -1, -1, 0 },
                          { -1, -1, 0, 0 },
                          { 0, -1, 0, 0 },
                          { 1, -1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 2) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }

  case 6: {
    int4 valueOne[4] = {
      { 0, -1, 0, 0 }, { 0, -2, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 }
    };
    int4 valueZero[9] = { { 1, 0, 1, 0 },
                          { 0, 0, 1, 0 },
                          { -1, 0, 1, 0 },
                          { -1, 1, 1, 0 },
                          { 0, 1, 1, 0 },
                          { 1, 1, 1, 0 },
                          { -1, 1, 0, 0 },
                          { 0, 1, 0, 0 },
                          { 1, 1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 4) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 7: {
    int4 valueOne[3] = { { 0, 1, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 } };
    int4 valueZero[9] = { { 1, 0, 1, 0 },
                          { 0, 0, 1, 0 },
                          { -1, 0, 1, 0 },
                          { -1, -1, 1, 0 },
                          { 0, -1, 1, 0 },
                          { 1, -1, 1, 0 },
                          { -1, -1, 0, 0 },
                          { 0, -1, 0, 0 },
                          { 1, -1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 8: {
    int4 valueOne[3] = { { 0, -1, 0, 0 }, { 0, -2, 0, 0 }, { 0, 0, 1, 0 } };
    int4 valueZero[9] = { { 1, 1, 0, 0 },
                          { 1, 0, 0, 0 },
                          { 1, -1, 0, 0 },
                          { 0, 1, -1, 0 },
                          { 0, 0, -1, 0 },
                          { 0, -1, -1, 0 },
                          { 1, 1, -1, 0 },
                          { 1, 0, -1, 0 },
                          { 1, -1, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 9: {
    int4 valueOne[2] = { { 0, 0, 1, 0 }, { 1, 0, 0, 0 } };
    int4 valueZero[9] = { { -1, 1, 0, 0 },
                          { -1, 0, 0, 0 },
                          { -1, -1, 0, 0 },
                          { 0, 1, -1, 0 },
                          { 0, 0, -1, 0 },
                          { 0, -1, -1, 0 },
                          { -1, 1, -1, 0 },
                          { -1, 0, -1, 0 },
                          { -1, -1, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 2) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 10: {
    int4 valueOne[4] = {
      { -1, 0, 0, 0 }, { -2, 0, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 }
    };
    int4 valueZero[9] = { { 1, 1, 1, 0 },
                          { 1, 0, 1, 0 },
                          { 1, -1, 1, 0 },
                          { 0, 1, 1, 0 },
                          { 0, 0, 1, 0 },
                          { 0, -1, 1, 0 },
                          { 1, 1, 0, 0 },
                          { 1, 0, 0, 0 },
                          { 1, -1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 4) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  case 11: {
    int4 valueOne[3] = { { 1, 0, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 } };
    int4 valueZero[9] = { { -1, 1, 1, 0 },
                          { -1, 0, 1, 0 },
                          { -1, -1, 1, 0 },
                          { 0, 1, 1, 0 },
                          { 0, 0, 1, 0 },
                          { 0, -1, 1, 0 },
                          { -1, 1, 0, 0 },
                          { -1, 0, 0, 0 },
                          { -1, -1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 9))
      return false;
    result = true;
    break;
  }
  }
  return result;
}
bool checkC(__read_only image3d_t imageIn, int4 coord, int structElVersion) {
  bool result = false;
  switch (structElVersion) {
  case 0: {
    int4 valueOne[4] = {
      { 0, 0, 1, 0 }, { 0, 1, 0, 0 }, { -1, 0, 0, 0 }, { -2, 0, 0, 0 }
    };

    int4 valueZero[7] = { { 1, 0, 0, 0 },
                          { 0, -1, 0, 0 },
                          { 1, -1, 0, 0 },

                          { 0, 0, -1, 0 },
                          { 0, -1, -1, 0 },
                          { 1, -1, -1, 0 },
                          { 1, 0, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 4) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 1: {
    int4 valueOne[3] = { { 0, 0, 1, 0 }, { 0, 1, 0, 0 }, { 1, 0, 0, 0 } };

    int4 valueZero[7] = { { -1, 0, 0, 0 },
                          { 0, -1, 0, 0 },
                          { -1, -1, 0, 0 },

                          { 0, 0, -1, 0 },
                          { 0, -1, -1, 0 },
                          { -1, -1, -1, 0 },
                          { -1, 0, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 2: {
    int4 valueOne[4] = {
      { 0, 0, 1, 0 }, { 1, 0, 0, 0 }, { 0, -1, 0, 0 }, { 0, -2, 0, 0 }
    };

    int4 valueZero[7] = { { 0, 1, 0, 0 },
                          { -1, 1, 0, 0 },
                          { -1, 0, 0, 0 },

                          { 0, 0, -1, 0 },
                          { 0, 1, -1, 0 },
                          { -1, 1, -1, 0 },
                          { -1, 0, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 4) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }

  case 3: {
    int4 valueOne[5] = { { -1, 0, 0, 0 },
                         { -2, 0, 0, 0 },
                         { 0, -1, 0, 0 },
                         { 0, -2, 0, 0 },
                         { 0, 0, 1, 0 } };

    int4 valueZero[7] = { { 0, 1, 0, 0 },
                          { 1, 1, 0, 0 },
                          { 1, 0, 0, 0 },
                          { 0, 0, -1, 0 },
                          { 0, 1, -1, 0 },
                          { 1, 1, -1, 0 },
                          { 1, 0, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 5) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 4: {
    int4 valueOne[5] = { { -1, 0, 0, 0 },
                         { -2, 0, 0, 0 },
                         { 0, 1, 0, 0 },
                         { 0, 0, -1, 0 },
                         { 0, 0, -2, 0 } };

    int4 valueZero[7] = { { 0, -1, 0, 0 },
                          { 1, -1, 0, 0 },
                          { 1, 0, 0, 0 },
                          { 0, 0, 1, 0 },
                          { 0, -1, 1, 0 },
                          { 1, -1, 1, 0 },
                          { 1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 5) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 5: {
    int4 valueOne[4] = {
      { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, -1, 0 }, { 0, 0, -2, 0 }
    };

    int4 valueZero[7] = { { 0, -1, 0, 0 },
                          { -1, -1, 0, 0 },
                          { -1, 0, 0, 0 },
                          { 0, 0, 1, 0 },
                          { 0, -1, 1, 0 },
                          { -1, -1, 1, 0 },
                          { -1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 4) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 6: {
    int4 valueOne[5] = { { 1, 0, 0, 0 },
                         { 0, -1, 0, 0 },
                         { 0, -2, 0, 0 },
                         { 0, 0, -1, 0 },
                         { 0, 0, -2, 0 } };

    int4 valueZero[7] = { { 0, 1, 0, 0 },
                          { -1, 1, 0, 0 },
                          { -1, 0, 0, 0 },
                          { 0, 0, 1, 0 },
                          { 0, 1, 1, 0 },
                          { -1, 1, 1, 0 },
                          { -1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 5) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 7: {
    int4 valueOne[6] = { { -1, 0, 0, 0 },
                         { -2, 0, 0, 0 },
                         { 0, -1, 0, 0 },
                         { 0, -2, 0, 0 },
                         { 0, 0, -1, 0 },
                         { 0, 0, -2, 0 } };

    int4 valueZero[7] = { { 0, 1, 0, 0 },
                          { 1, 1, 0, 0 },
                          { 1, 0, 0, 0 },
                          { 0, 0, 1, 0 },
                          { 0, 1, 1, 0 },
                          { 1, 1, 1, 0 },
                          { 1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 6) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  }
  return result;
}
bool checkD(__read_only image3d_t imageIn, int4 coord, int structElVersion) {
  bool result = false;
  switch (structElVersion) {
  case 0: {
    int4 valueOne[1] = { { 0, -1, 1, 0 } };
    int4 valueAny[5] = { { 0, -2, 0, 0 },
                         { 0, -2, 1, 0 },
                         { 0, -2, 2, 0 },
                         { 0, -1, 2, 0 },
                         { 0, 0, 2, 0 } };
    int4 valueZero[17] = { { 0, 0, -1, 0 },
                           { 1, 0, -1, 0 },
                           { 1, 1, -1, 0 },
                           { 0, 1, -1, 0 },
                           { -1, 0, -1, 0 },
                           { -1, -1, -1, 0 },
                           { 0, -1, -1, 0 },
                           { -1, 1, -1, 0 },
                           { 1, -1, -1, 0 },

                           { 0, -1, 0, 0 },
                           { -1, 1, 0, 0 },
                           { 0, 1, 0, 0 },
                           { 1, 1, 0, 0 },

                           { 0, 0, 1, 0 },
                           { 0, 1, 1, 0 },
                           { 1, 1, 1, 0 },
                           { -1, 1, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkAny(imageIn, coord, valueAny, 5) ||
        !checkZero(imageIn, coord, valueZero, 17))
      return false;
    result = true;
    break;
  }
  case 1: {
    int4 valueOne[1] = { { 0, -1, -1, 0 } };
    int4 valueAny[5] = { { 0, -2, 0, 0 },
                         { 0, -2, -1, 0 },
                         { 0, -2, -2, 0 },
                         { 0, -1, -2, 0 },
                         { 0, 0, -2, 0 } };
    int4 valueZero[17] = { { 0, 0, 1, 0 },
                           { 1, 0, 1, 0 },
                           { 1, 1, 1, 0 },
                           { 0, 1, 1, 0 },
                           { -1, 0, 1, 0 },
                           { -1, -1, 1, 0 },
                           { 0, -1, 1, 0 },
                           { -1, 1, 1, 0 },
                           { 1, -1, 1, 0 },

                           { -1, 1, 0, 0 },
                           { 0, 1, 0, 0 },
                           { 1, 1, 0, 0 },
                           { 0, -1, 0, 0 },

                           { 0, 0, -1, 0 },
                           { 0, 1, -1, 0 },
                           { 1, 1, -1, 0 },
                           { -1, 1, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkAny(imageIn, coord, valueAny, 5) ||
        !checkZero(imageIn, coord, valueZero, 17))
      return false;
    result = true;
    break;
  }
  case 2: {
    int4 valueOne[1] = { { -1, -1, 0, 0 } };
    int4 valueAny[5] = { { -2, 0, 0, 0 },
                         { -2, -1, 0, 0 },
                         { -2, -2, 0, 0 },
                         { -1, -2, 0, 0 },
                         { 0, -2, 0, 0 } };
    int4 valueZero[16] = { { 1, 1, 0, 0 },
                           { 0, 1, 0, 0 },
                           { -1, 1, 0, 0 },
                           { 1, 1, -1, 0 },
                           { 0, 1, -1, 0 },
                           { -1, 1, -1, 0 },
                           { 1, 1, 1, 0 },
                           { 0, 1, 1, 0 },
                           { -1, 1, 1, 0 },

                           { 1, -1, -1, 0 },
                           { 1, 0, -1, 0 },
                           { 1, 0, 0, 0 },
                           { 1, -1, 0, 0 },
                           { 1, 0, 1, 0 },

                           { -1, 0, 0, 0 },
                           { 0, -1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkAny(imageIn, coord, valueAny, 5) ||
        !checkZero(imageIn, coord, valueZero, 16))
      return false;
    result = true;
    break;
  }
  case 3: {
    int4 valueOne[1] = { { 1, 1, 0, 0 } };
    int4 valueAny[5] = { { 2, 0, 0, 0 },
                         { 2, -1, 0, 0 },
                         { 2, -2, 0, 0 },
                         { 1, -2, 0, 0 },
                         { 0, -2, 0, 0 } };
    int4 valueZero[17] = { { 1, 1, 0, 0 },
                           { 0, 1, 0, 0 },
                           { -1, 1, 0, 0 },
                           { 1, 1, -1, 0 },
                           { 0, 1, -1, 0 },
                           { -1, 1, -1, 0 },
                           { 1, 1, 1, 0 },
                           { 0, 1, 1, 0 },
                           { -1, 1, 1, 0 },

                           { -1, 0, 1, 0 },
                           { -1, 0, -1, 0 },
                           { -1, 0, 0, 0 },
                           { -1, -1, 0, 0 },
                           { -1, -1, -1, 0 },
                           { -1, -1, 1, 0 },

                           { 1, 0, 0, 0 },
                           { 0, -1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkAny(imageIn, coord, valueAny, 5) ||
        !checkZero(imageIn, coord, valueZero, 17))
      return false;
    result = true;
    break;
  }
  case 4: {
    int4 valueOne[1] = { { -1, 0, 1, 0 } };
    int4 valueAny[5] = { { 0, 0, 2, 0 },
                         { -1, 0, 2, 0 },
                         { -2, 0, 2, 0 },
                         { -2, 0, 1, 0 },
                         { -2, 0, 0, 0 } };
    int4 valueZero[17] = { { 1, 0, 0, 0 },
                           { 1, 0, 1, 0 },
                           { 1, 0, -1, 0 },
                           { 1, 1, 0, 0 },
                           { 1, 1, 1, 0 },
                           { 1, 1, -1, 0 },
                           { 1, -1, 0, 0 },
                           { 1, -1, 1, 0 },
                           { 1, -1, -1, 0 },

                           { -1, 0, -1, 0 },
                           { -1, 1, -1, 0 },
                           { -1, -1, -1, 0 },
                           { 0, 0, -1, 0 },
                           { 0, 1, -1, 0 },
                           { 0, -1, -1, 0 },

                           { 0, 0, 1, 0 },
                           { -1, 0, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkAny(imageIn, coord, valueAny, 5) ||
        !checkZero(imageIn, coord, valueZero, 17))
      return false;
    result = true;
    break;
  }
  case 5: {
    int4 valueOne[1] = { { -1, 0, -1, 0 } };
    int4 valueAny[5] = { { 0, 0, -2, 0 },
                         { -1, 0, -2, 0 },
                         { -2, 0, -2, 0 },
                         { -2, 0, -1, 0 },
                         { -2, 0, 0, 0 } };
    int4 valueZero[17] = { { 1, 0, 0, 0 },
                           { 1, 0, 1, 0 },
                           { 1, 0, -1, 0 },
                           { 1, 1, 0, 0 },
                           { 1, 1, 1, 0 },
                           { 1, 1, -1, 0 },
                           { 1, -1, 0, 0 },
                           { 1, -1, 1, 0 },
                           { 1, -1, -1, 0 },

                           { -1, 0, 1, 0 },
                           { -1, 1, 1, 0 },
                           { -1, -1, 1, 0 },
                           { 0, 0, 1, 0 },
                           { 0, 1, 1, 0 },
                           { 0, -1, 1, 0 },

                           { 0, 0, -1, 0 },
                           { -1, 0, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 1) ||
        !checkAny(imageIn, coord, valueAny, 5) ||
        !checkZero(imageIn, coord, valueZero, 17))
      return false;
    result = true;
    break;
  }
  case 6: {
    int4 valueOne[3] = { { 0, 0, -1, 0 }, { 0, 0, 1, 0 }, { 1, 1, 0, 0 } };
    int4 valueZero[7] = { { 0, -1, 0, 0 },
                          { 1, -1, 0, 0 },
                          { -1, -1, 0, 0 },
                          { -1, 0, 0, 0 },
                          { 1, 0, 0, 0 },
                          { -1, 1, 0, 0 },
                          { 0, 1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 7: {
    int4 valueOne[3] = { { 0, 0, -1, 0 }, { 0, 0, 1, 0 }, { -1, 1, 0, 0 } };
    int4 valueZero[7] = { { 0, -1, 0, 0 },
                          { 1, -1, 0, 0 },
                          { -1, -1, 0, 0 },
                          { -1, 0, 0, 0 },
                          { 1, 0, 0, 0 },
                          { 1, 1, 0, 0 },
                          { 0, 1, 0, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 8: {
    int4 valueOne[3] = { { -1, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 1, 1, 0 } };
    int4 valueZero[7] = { { 0, 0, -1, 0 },
                          { 0, -1, -1, 0 },
                          { 0, 1, -1, 0 },
                          { 0, -1, 0, 0 },
                          { 0, 1, 0, 0 },
                          { 0, -1, 1, 0 },
                          { 0, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 9: {
    int4 valueOne[3] = { { -1, 0, 0, 0 }, { 1, 0, 0, 0 }, { 0, 1, -1, 0 } };
    int4 valueZero[7] = { { 0, 0, -1, 0 },
                          { 0, -1, -1, 0 },
                          { 0, -1, 0, 0 },
                          { 0, 1, 0, 0 },
                          { 0, 1, 1, 0 },
                          { 0, -1, 1, 0 },
                          { 0, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 10: {
    int4 valueOne[3] = { { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 1, 0, -1, 0 } };
    int4 valueZero[7] = { { 0, 0, 1, 0 },
                          { 1, 0, 1, 0 },
                          { -1, 0, 1, 0 },
                          { 1, 0, 0, 0 },
                          { -1, 0, 0, 0 },
                          { -1, 0, -1, 0 },
                          { 0, 0, -1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 7))
      return false;
    result = true;
    break;
  }
  case 11: {
    int4 valueOne[3] = { { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 1, 0, 1, 0 } };
    int4 valueZero[6] = { { 0, 0, -1, 0 },
                          { 1, 0, -1, 0 },
                          { -1, 0, -1, 0 },
                          { 1, 0, 0, 0 },
                          { -1, 0, 0, 0 },
                          { -1, 0, 1, 0 } };
    if (!checkOne(imageIn, coord, valueOne, 3) ||
        !checkZero(imageIn, coord, valueZero, 6))
      return false;
    result = true;
    break;
  }
  }
  return result;
}
__kernel void Skeletonize3d(__read_only image3d_t imageIn,
                            __write_only image3d_t imageOut, int template,
                            int structElVersion) {
  int4 coord = (int4){ get_global_id(0), get_global_id(1), get_global_id(2),
                       get_global_id(3) };
  float outValue = read_imagef(imageIn, sampler, coord).x;
  if (outValue == 0) {
    write_imagef(imageOut, coord, outValue);
    return;
  }
  switch (template) {
  case 0:
    if (checkA(imageIn, coord, structElVersion))
      outValue = 0;
    break;
  case 1:
    if (checkB(imageIn, coord, structElVersion))
      outValue = 0;
    break;
  case 2:
    if (checkC(imageIn, coord, structElVersion))
      outValue = 0;
    break;
  case 3:
    if (checkD(imageIn, coord, structElVersion))
      outValue = 0;
    break;
  }

  write_imagef(imageOut, coord, outValue);
}
