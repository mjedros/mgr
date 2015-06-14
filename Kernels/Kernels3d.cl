constant sampler_t sampler =
    CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_ADDRESS_CLAMP;

#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

__kernel void Erode3dEllipse(__read_only image3d_t imageIn,
                             __write_only image3d_t imageOut,
                             const float3 ellipseParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;
  write_imagef(imageOut, image_coord, inPixel);
}
__kernel void Dilate3dEllipse(__read_only image3d_t imageIn,
                              __write_only image3d_t imageOut,
                              const float3 ellipseParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;
  ;
  write_imagef(imageOut, image_coord, inPixel);
}

__kernel void Erode3dRectangle(__read_only image3d_t imageIn,
                               __write_only image3d_t imageOut,
                               const float3 ellipseParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;
  write_imagef(imageOut, image_coord, inPixel);
}
__kernel void Dilate3dRectangle(__read_only image3d_t imageIn,
                                __write_only image3d_t imageOut,
                                const float3 ellipseParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  for (int i = -ellipseParams.x; i <= ellipseParams.x; i++) {
    for (int j = -ellipseParams.y; j <= ellipseParams.y; j++) {
      for (int k = -ellipseParams.z; k <= ellipseParams.z; k++) {
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
                           const float3 ellipseParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;
  write_imagef(imageOut, image_coord, inPixel);
}

__kernel void Dilate3dCross(__read_only image3d_t imageIn,
                            __write_only image3d_t imageOut,
                            const float3 ellipseParams) {
  int4 image_coord = (int4){ get_global_id(0), get_global_id(1),
                             get_global_id(2), get_global_id(3) };
  float inPixel = read_imagef(imageIn, sampler, image_coord).x;
  write_imagef(imageOut, image_coord, inPixel);
}
