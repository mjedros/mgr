constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_REPEAT | CLK_FILTER_NEAREST;

__kernel void sobel_rgb(__read_only image2d_t imageIn, __write_only image2d_t imageOut) {
   float out_color = 0.0;

   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};

   float pixel = read_imagef(imageIn, sampler, image_coord).x;
   if (pixel > 0.3f) {
      out_color = 0;
   } else {
      out_color = 1;
   }
   write_imagef(imageOut, image_coord, out_color);
}
__constant int squareSize = 40;
__kernel void Dilate(__read_only image2d_t imageIn, __write_only image2d_t imageOut) {
   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};
   float3 ellipseParams = {0, 10, 10};
   float sine = sin(ellipseParams.x);
   float cosine = cos(ellipseParams.x);
   float Ri = ellipseParams.y;
   float Rj = ellipseParams.z;
   for (int i = -squareSize; i < squareSize; i++) {
      for (int j = -squareSize; j < squareSize; j++) {
         float m = i * cosine - j * sine;
         float n = i * sine + j * cosine;
         if (m * m + (Ri * Ri / Rj / Rj) * n * n < Ri * Ri) {
            int2 ijCoord = image_coord + (int2) {i, j};
            float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
            if (inPixel > 0.5) {
               write_imagef(imageOut, image_coord, 1);
               return;
            }
         }
      }
   }
   write_imagef(imageOut, image_coord, 0);
}
