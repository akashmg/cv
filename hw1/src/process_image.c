#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

void pad_index(int* in, int lim)
{
    if (*in < 0)
    {
        *in = 0;
    }
    else if (*in >= lim)
    {
        *in = lim - 1;
    }
}

float get_pixel(image im, int x, int y, int c)
{
    int max_ind = im.w * im.h * im.c - 1;
    // printf("w=%d h=%d c=%d\n", im.w, im.h, im.c);
    pad_index(&c, im.c);
    pad_index(&x, im.w);
    pad_index(&y, im.h);
    int ind = c * im.w * im.h + y * im.w + x;
    // printf("padded: w=%d h=%d c=%d\n", x, y, c);
    if (ind < 0)
    {
        ind = 0;
    }
    else if (ind > max_ind)
    {
        ind = max_ind;
    }
    else
    {
        // index is valid
    }

    return im.data[ind];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    int ind = c * im.w * im.h + y * im.w + x;
    int max_ind = im.w * im.h * im.c - 1;
    if (ind < 0 || ind > max_ind) {
        return;
    }
    im.data[ind] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, (im.w * im.h * im.c)*sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    int pg_offset = im.w * im.h;
    for (int i = 0; i < im.h; i++)
    {
        int row_offset = im.w * i;
        for (int j = 0; j < im.w; j++)
        {
            gray.data[row_offset + j] = im.data[j] * 0.299 +
                                        im.data[j + pg_offset] * 0.587 +
                                        im.data[j + 2 * pg_offset] * 0.114;
        }
    }

    return gray;
}

void shift_image(image im, int c, float v)
{
    int pg_offset = im.w * im.h * c;
    for (int i = 0; i < im.h; i++)
    {
        int row_offset = im.w * i;
        for (int j = 0; j < im.w; j++)
        {
            im.data[pg_offset + row_offset + j] += v;
        }
    }
}

void scale_image(image im, int c, float v)
{
    int pg_offset = im.w * im.h * c;
    for (int i = 0; i < im.h; i++)
    {
        int row_offset = im.w * i;
        for (int j = 0; j < im.w; j++)
        {
            im.data[pg_offset + row_offset + j] *= v;
        }
    }
}

void clamp_image(image im)
{
    for (int i = 0; i < im.c; i++)
    {
        int pg_offset = im.w * im.h * i;
        for (int j = 0; j < im.h; j++)
        {
            int row_offset = im.w * j;
            for (int k = 0; k < im.w; k++)
            {
                int ind = pg_offset + row_offset + k;
                if (im.data[ind] < 0)
                {
                    im.data[ind] = 0;
                }
                else if (im.data[ind] > 1)
                {
                    im.data[ind] = 1;
                }
                else
                {
                    // do nothing
                }
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    int r;
    int g;
    int b;
    float V;
    float m;
    float C;
    float S;
    float H = 0;
    int pg_offset = im.w * im.h;
    for (int i = 0; i < im.h; i++)
    {
        int row_offset = im.w * i;
        for (int j = 0; j < im.w; j++)
        {
            r = row_offset + j;
            g = pg_offset + r;
            b = pg_offset + g;
            V = three_way_max(im.data[r], im.data[g], im.data[b]);
            m = three_way_min(im.data[r], im.data[g], im.data[b]);
            C = V - m;
            if (V == 0)
            {
                S = 0;
            }
            else
            {
                S = C / V;
            }
            if (C == 0)
            {
                H = 0;
            }
            else if (V == im.data[r])
            {
                H = (im.data[g] - im.data[b]) / C;
            }
            else if (V == im.data[g])
            {
                H = (im.data[b] - im.data[r]) / C + 2;
            }
            else if (V == im.data[b])
            {
                H = (im.data[r] - im.data[g]) / C + 4;
            }

            if (H < 0)
            {
                H = 1 + H / 6;
            }
            else
            {
                H = H / 6;
            }
            im.data[r] = H;
            im.data[g] = S;
            im.data[b] = V;
        }
    }
}

float kcalc(int n, float h, float s, float v)
{
    float k = fmod((n + h * 6), 6);
    float min = three_way_min(k, 4 - k, 1);
    float max = min > 0 ? min : 0;
    return (v - v * s * max);
}

void hsv_to_rgb(image im)
{
    int pg_offset, row_offset;
    float r = 0, g = 0, b = 0;
    int h_ind, s_ind, v_ind;
    float h, s, v;
    pg_offset = im.w * im.h;
    for (int i = 0; i < im.h; i++)
    {
        row_offset = im.w * i;
        for (int j = 0; j < im.w; j++)
        {
            h_ind = row_offset + j;
            s_ind = pg_offset + h_ind;
            v_ind = pg_offset + s_ind;
            h = im.data[h_ind];
            s = im.data[s_ind];
            v = im.data[v_ind];
            r = kcalc(5, h, s, v);
            g = kcalc(3, h, s, v);
            b = kcalc(1, h, s, v);
            im.data[h_ind] = r;
            im.data[s_ind] = g;
            im.data[v_ind] = b;
        }  
    }
}

void hsv_to_rgb_needs_work(image im)
{
    int pg_offset, row_offset;
    float r = 0, g = 0, b = 0;
    int h_ind, s_ind, v_ind;
    float h, s, v;
    float m, C, x;

    pg_offset = im.w * im.h;
    for (int i = 0; i < im.h; i++)
    {
        row_offset = im.w * i;
        for (int j = 0; j > im.w; j++)
        {
            h_ind = row_offset + j;
            s_ind = pg_offset + h_ind;
            v_ind = pg_offset + s_ind;
            h = im.data[h_ind];
            s = im.data[s_ind];
            v = im.data[v_ind];
            C = s * v;
            m = v - C;
            h = h * 6;
            x = C * (1 - fabs(fmod(h, 2.0) - 1));
            if (h <= 1)
            {
                r = C;
                g = x;
                b = 0;
            }
            else if (h <= 2)
            {
                r = x;
                g = C;
                b = 0;
            }
            else if (h <= 3)
            {
                r = 0;
                g = C;
                b = x;
            }
            else if (h <= 4)
            {
                r = 0;
                g = x;
                b = C;
            }
            else if (h <= 5)
            {
                r = x;
                g = 0;
                b = C;
            }
            else if (h <= 6)
            {
                r = C;
                g = 0;
                b = x;
            }

            im.data[h_ind] = r + m;
            im.data[s_ind] = g + m;
            im.data[v_ind] = b + m;
        }
    }
}

void rgb_to_hcl(image im)
{
    
}
