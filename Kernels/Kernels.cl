constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_REPEAT | CLK_FILTER_NEAREST;

__kernel void threshold(__read_only image2d_t imageIn, __write_only image2d_t imageOut,
                        const float threshold)
{
   float out_color = 0.0;

   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};

   float pixel = read_imagef(imageIn, sampler, image_coord).x;
   if (pixel > threshold)
   {
      out_color = 0;
   }
   else
   {
      out_color = 1;
   }
   write_imagef(imageOut, image_coord, out_color);
}
__constant int squareSize = 10;
__kernel void Erode(__read_only image2d_t imageIn, __write_only image2d_t imageOut,
                    const float3 ellipseParams)
{
   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};
   float sine = sin(ellipseParams.x);
   float cosine = cos(ellipseParams.x);
   float Ri = ellipseParams.y;
   float Rj = ellipseParams.z;

   for (int i = -squareSize; i < squareSize; i++)
   {
      for (int j = -squareSize; j < squareSize; j++)
      {
         float m = i * cosine - j * sine;
         float n = i * sine + j * cosine;
         if (m * m + (Ri * Ri / Rj / Rj) * n * n < Ri * Ri)
         {
            int2 ijCoord = image_coord + (int2) {i, j};
            float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
            if (inPixel == 0)
            {
               write_imagef(imageOut, image_coord, 0);
               return;
            }
         }
      }
   }
   write_imagef(imageOut, image_coord, 1);
}
__kernel void Dilate(__read_only image2d_t imageIn, __write_only image2d_t imageOut,
                     const float3 ellipseParams)
{
   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};
   float sine = sin(ellipseParams.x);
   float cosine = cos(ellipseParams.x);
   float Ri = ellipseParams.y;
   float Rj = ellipseParams.z;

   for (int i = -squareSize; i < squareSize; i++)
   {
      for (int j = -squareSize; j < squareSize; j++)
      {
         float m = i * cosine - j * sine;
         float n = i * sine + j * cosine;
         if (m * m + (Ri * Ri / Rj / Rj) * n * n < Ri * Ri)
         {
            int2 ijCoord = image_coord + (int2) {i, j};
            float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
            if (inPixel == 1)
            {
               write_imagef(imageOut, image_coord, 1);
               return;
            }
         }
      }
   }
   write_imagef(imageOut, image_coord, 0);
}

__kernel void ErodeRect(__read_only image2d_t imageIn, __write_only image2d_t imageOut,
                        const int2 rectangle)
{
   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};
   for (int i = -rectangle.x; i <= rectangle.x; i++)
   {
      for (int j = -rectangle.y; j <= rectangle.y; j++)
      {
         int2 ijCoord = image_coord + (int2) {i, j};
         float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
         if (inPixel == 0)
         {
            write_imagef(imageOut, image_coord, 0);
            return;
         }
      }
   }
   write_imagef(imageOut, image_coord, 1);
}
__kernel void DilateRect(__read_only image2d_t imageIn, __write_only image2d_t imageOut,
                         const int2 rectangle)
{
   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};
   for (int i = -rectangle.x; i <= rectangle.x; i++)
   {
      for (int j = -rectangle.y; j <= rectangle.y; j++)
      {
         int2 ijCoord = image_coord + (int2) {i, j};
         float inPixel = read_imagef(imageIn, sampler, ijCoord).x;
         if (inPixel == 1)
         {
            write_imagef(imageOut, image_coord, 1);
            return;
         }
      }
   }
   write_imagef(imageOut, image_coord, 0);
}

__kernel void ErodeCross(__read_only image2d_t imageIn, __write_only image2d_t imageOut,
                         const int2 rectangle)
{
   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};
   write_imagef(imageOut, image_coord, 1);
   for (int i = -rectangle.x; i <= rectangle.x; i++)
   {
      int hor = 0;
      if (i == 0)
      {
         hor = rectangle.y;
      }

      for (int j = -hor; j <= hor; j++)
      {
         if (read_imagef(imageIn, sampler, image_coord + (int2) {i, j}).x == 0)
         {
            write_imagef(imageOut, image_coord, 0);
            return;
         }
      }
   }
}

__kernel void DilateCross(__read_only image2d_t imageIn, __write_only image2d_t imageOut,
                          const int2 rectangle)
{
   int2 image_coord = (int2) {get_global_id(0), get_global_id(1)};
   write_imagef(imageOut, image_coord, 0);
   for (int i = -rectangle.x; i <= rectangle.x; i++)
   {
      int hor = 0;
      if (i == 0)
      {
         hor = rectangle.y;
      }

      for (int j = -hor; j <= hor; j++)
      {
         if (read_imagef(imageIn, sampler, image_coord + (int2) {i, j}).x == 1)
         {
            write_imagef(imageOut, image_coord, 1);
            return;
         }
      }
   }
}
