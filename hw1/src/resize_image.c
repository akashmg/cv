#include <math.h>
#include "image.h"
#include <stdio.h>
#include <stdlib.h>

float nn_interpolate(image im, float x, float y, int c)
{
    int x1 = x + 0.5;
    int y1 = y + 0.5;
    // printf("%f %d %f %d\n", x, x1, y, y1);
    return get_pixel(im, x1, y1, c);
}

image nn_resize(image im, int w, int h)
{
    image im_new = make_image(w, h, im.c);

    float ax = (float)im.w / (float)w;
    float bx = -0.5 + 0.5 * ax;
    float ay = (float)im.h / (float)h;
    float by = -0.5 + 0.5 * ay;

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            for (int k = 0; k < im.c; k++)
            {
                float x = ax * (float)i + bx;
                float y = ay * (float)j + by;
                float val = nn_interpolate(im, x, y, k);
                set_pixel(im_new, i, j, k, val);
            }
        }
    }

    return im_new;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    int x1 = floor(x);
    int y1 = floor(y);
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    float l = x - x1;
    float b = y - y1;

    float p1 = get_pixel(im, x1, y1, c);
    float p2 = get_pixel(im, x2, y1, c);
    float p3 = get_pixel(im, x1, y2, c);
    float p4 = get_pixel(im, x2, y2, c);

    float p = l * b * p4 +
              (1 - l) * b * p3 +
              l * (1 - b) * p2 +
              (1 - l) * (1 - b) * p1;

    return p;
}

image bilinear_resize(image im, int w, int h)
{
    image im_new = make_image(w, h, im.c);
    float ax = (float)im.w / (float)w;
    float bx = -0.5 + 0.5 * ax;
    float ay = (float)im.h / (float)h;
    float by = -0.5 + 0.5 * ay;

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            for (int k = 0; k < im.c; k++)
            {
                float x = ax * (float)i + bx;
                float y = ay * (float)j + by;
                float val = bilinear_interpolate(im, x, y, k);
                set_pixel(im_new, i, j, k, val);
            }
        }
    }
    return im_new;
}

