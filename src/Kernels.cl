constant sampler_t sampler =
    CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_REPEAT | CLK_FILTER_NEAREST;

__kernel void sobel_rgb(__read_only image2d_t imageIn,
                        __write_only image2d_t imageOut) {
  float out_color = 0.0;

  int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};

  float pixel = read_imagef(imageIn, sampler, image_coord).x;
  if (pixel > 0.3f) {
    out_color = 1;
  } else {
    out_color = 0;
  }
  write_imagef(imageOut, image_coord, out_color);
}
