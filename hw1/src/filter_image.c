#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0;
    float offset = 0;
    float scale = 0;
    for (int i = 0; i < im.w; i++)
    {
        for (int j = 0; j < im.h; j++)
        {
            for (int k = 0; k < im.c; k++)
            {
                sum += get_pixel(im, i, j, k);
            }
        }
    }
    if (sum == 0)
    {
        sum = im.w * im.h;
        offset = 1/sum; 
    }
    else
    {
        scale = 1/sum;
    }
    for (int i = 0; i < im.w; i++)
    {
        for (int j = 0; j < im.h; j++)
        {
            for (int k = 0; k < im.c; k++)
            {
                float val = get_pixel(im, i, j, k);
                set_pixel(im, i, j, k, val*scale + offset);
            }
        }
    }
}

image make_box_filter(int w)
{
    image box_filt = make_image(w, w, 1);
    l1_normalize(box_filt);
    return box_filt;
}

image convolve_image(image im, image filter, int preserve)
{
    assert(filter.c == 1 || filter.c == im.c);
    int filt_mid_x = filter.w/2;
    int filt_mid_y = filter.h/2;
    image im_new;

    if (preserve == 1)
    {
        im_new = make_image(im.w, im.h, im.c);
    }
    else
    {
        im_new = make_image(im.w, im.h, 1);
    }

    for (int i = 0; i < im.w; i++)
    {
        for (int j = 0; j < im.h; j++)
        {
            float v[im.c];
            float vsum = 0;
            for (int pg = 0; pg < im.c; pg++)
            {
                v[pg] = 0;
                for (int k = 0; k < filter.w; k++)
                {
                    for (int l = 0; l < filter.h; l++)
                    {
                        float fval = get_pixel(filter, k, l, pg);
                        int im_filt_x = k - filt_mid_x + i;
                        int im_filt_y = l - filt_mid_y + j;
                        float imval = get_pixel(im, im_filt_x, im_filt_y, pg);
                        v[pg] += fval * imval;
                    }
                }
                // If preserving structure then set vals; otherwise sum between pgs
                if (preserve == 1)
                {
                    set_pixel(im_new, i, j, pg, v[pg]);
                }
                else
                {
                    vsum += v[pg];
                }
            }
            if (preserve == 0)
            {
                set_pixel(im_new, i, j, 0, vsum);
                // printf("%f %f\n", vsum, im_new.data[i + j*im.w]);
            }
        }
    }

    return im_new;
}

image make_highpass_filter()
{
    image filt = make_image(3, 3, 1);

    filt.data[0] = 0;
    filt.data[1] = -1;
    filt.data[2] = 0;
    filt.data[3] = -1;
    filt.data[4] = 4;
    filt.data[5] = -1;
    filt.data[6] = 0;
    filt.data[7] = -1;
    filt.data[8] = 0;

    return filt;
}

image make_sharpen_filter()
{
    image filt = make_image(3, 3, 1);

    filt.data[0] = 0;
    filt.data[1] = -1;
    filt.data[2] = 0;
    filt.data[3] = -1;
    filt.data[4] = 5;
    filt.data[5] = -1;
    filt.data[6] = 0;
    filt.data[7] = -1;
    filt.data[8] = 0;

    return filt;
}

image make_emboss_filter()
{
    image filt = make_image(3, 3, 1);

    filt.data[0] = -2;
    filt.data[1] = -1;
    filt.data[2] = 0;
    filt.data[3] = -1;
    filt.data[4] = 1;
    filt.data[5] = 1;
    filt.data[6] = 0;
    filt.data[7] = 1;
    filt.data[8] = 2;

    return filt;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: Preserve when we need color. So emboss and sharpen.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: For all filters. Clamp values to [0,1] range.

image make_gaussian_filter(float sigma)
{
    int sigma_rounded = sigma * 6 + 0.5;
    if (sigma_rounded % 2 == 0) {
        sigma_rounded += 1;
    }
    float inv_sig_sq = 1/sigma/sigma;
    float k = (1/TWOPI)*inv_sig_sq;
    image filt = make_image(sigma_rounded, sigma_rounded, 1);
    int mid_p = sigma_rounded/2;

    for (int i = 0; i < sigma_rounded; i++)
    {
        float i_s = i - mid_p;
        for (int j = 0; j < sigma_rounded; j++)
        {
            float j_s = j - mid_p;
            float ind = -(float)(i_s*i_s + j_s*j_s)/2.0*inv_sig_sq;
            float val = k * exp(ind);
            set_pixel(filt, i, j, 0, val);
        }
    }

    l1_normalize(filt);

    return filt;
}

image add_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image im_add = make_image(a.w, a.h, a.c);

    for (int i = 0; i < a.w; i++)
    {
        for (int j = 0; j < a.h; j++)
        {
            for (int k = 0; k < a.c; k++)
            {
                float v_a = get_pixel(a, i, j, k);
                float v_b = get_pixel(b, i, j, k);
                set_pixel(im_add, i, j, k, v_a + v_b);
            }
        }
    }

    return im_add;
}

image sub_image(image a, image b)
{
    assert(a.w == b.w && a.h == b.h && a.c == b.c);
    image im_sub = make_image(a.w, a.h, a.c);

    for (int i = 0; i < a.w; i++)
    {
        for (int j = 0; j < a.h; j++)
        {
            for (int k = 0; k < a.c; k++)
            {
                float v_a = get_pixel(a, i, j, k);
                float v_b = get_pixel(b, i, j, k);
                set_pixel(im_sub, i, j, k, v_a - v_b);
            }
        }
    }

    return im_sub;
}

image make_gx_filter()
{
    image filt = make_image(3, 3, 1);

    filt.data[0] = -1;
    filt.data[1] = 0;
    filt.data[2] = 1;
    filt.data[3] = -2;
    filt.data[4] = 0;
    filt.data[5] = 2;
    filt.data[6] = -1;
    filt.data[7] = 0;
    filt.data[8] = 1;

    return filt;
}

image make_gy_filter()
{
    image filt = make_image(3, 3, 1);

    filt.data[0] = -1;
    filt.data[1] = -2;
    filt.data[2] = -1;
    filt.data[3] = 0;
    filt.data[4] = 0;
    filt.data[5] = 0;
    filt.data[6] = 1;
    filt.data[7] = 2;
    filt.data[8] = 1;

    return filt;
}

void feature_normalize(image im)
{
    float v_min = 0;
    float v_max = 0;

    for (int i = 0; i < im.w; i++)
    {
        for (int j = 0; j < im.h; j++)
        {
            for (int k = 0; k < im.c; k++)
            {
                float v = get_pixel(im, i, j, k);
                if (v < v_min)
                {
                    v_min = v;
                }
                if (v > v_max)
                {
                    v_max = v;
                }
            }
        }
    }

    v_max -= v_min;

    // if vmax is zero then the min and max values were the same
    // so (v - v_min) == 0; set v_max to 1 to protect from div by 0
    if (v_max == 0)
    {
        v_max = 1;
    }
    for (int i = 0; i < im.w; i++)
    {
        for (int j = 0; j < im.h; j++)
        {
            for (int k = 0; k < im.c; k++)
            {
                float v = get_pixel(im, i, j, k);
                v -= v_min;
                v /= v_max;
                set_pixel(im, i, j, k, v);
            }
        }
    }
}

image *sobel_image(image im)
{
    image gxf = make_gx_filter();
    image gyf = make_gy_filter();
    image gx = convolve_image(im, gxf, 0);
    image gy = convolve_image(im, gyf, 0);
    image *g = calloc(2, sizeof(image));
    g[0].w = im.w;
    g[0].h = im.h;
    g[0].c = 1;
    g[0].data = calloc(im.w * im.h * 1, sizeof(float));
    g[1].w = im.w;
    g[1].h = im.h;
    g[1].c = 1;
    g[1].data = calloc(im.w * im.h * 1, sizeof(float));

    for (int i = 0; i < im.w; i++)
    {
        for (int j = 0; j < im.h; j++)
        {
            for (int k = 0; k < 1; k++)
            {
                float vgx = get_pixel(gx, i, j, k);
                float vgy = get_pixel(gy, i, j, k);
                // printf("%f %f\n", vgy, vgx);
                float v = sqrt(vgx * vgx + vgy * vgy);
                if (vgx == 0)
                {
                    vgx = 1e-12;
                }
                float t = atan2(vgy, vgx);
                set_pixel(g[0], i, j, k, v);
                set_pixel(g[1], i, j, k, t);
            }
        }
    }

    return g;
}

image colorize_sobel(image im)
{
    image *res = sobel_image(im);
    image mag = res[0];
    image theta = res[1];
    feature_normalize(mag);
    feature_normalize(theta);
    image color = make_image(im.w, im.h, im.c);
    for (int i = 0; i < im.w; i++)
    {
        for (int j = 0; j < im.h; j++)
        {
            for (int k = 0; k < im.c; k++)
            {
                if (k == 1)
                {
                    set_pixel(color, i, j, k, get_pixel(theta, i, j, 0));
                }
                else
                {
                    set_pixel(color, i, j, k, get_pixel(mag, i, j, 0));
                }
            }
        }
    }
    hsv_to_rgb(color);

    return color;
}
