# 1. Getting and setting pixels
from uwimg import *
# im = load_image("data/dogsmall.jpg")
# a = nn_resize(im, im.w*4, im.h*4)
# save_image(a, "dog4x-nn")

# im = load_image("data/dogsmall.jpg")
# a = bilinear_resize(im, im.w*4, im.h*4)
# save_image(a, "dog4x-bl")

# im = load_image("data/dog.jpg")
# a = nn_resize(im, im.w//7, im.h//7)
# save_image(a, "dog7th-bl")

# im = load_image("data/dog.jpg")
# f = make_box_filter(7)
# blur = convolve_image(im, f, 1)
# save_image(blur, "dog-box7")

# im = load_image("data/dog.jpg")
# f = make_highpass_filter()
# blur = convolve_image(im, f, 0)
# clamp_image(blur);
# save_image(blur, "dog-highpass")

# im = load_image("data/dog.jpg")
# f = make_emboss_filter()
# blur = convolve_image(im, f, 1)
# clamp_image(blur);
# save_image(blur, "dog-emboss")

# im = load_image("data/dog.jpg")
# f = make_sharpen_filter()
# blur = convolve_image(im, f, 1)
# clamp_image(blur);
# save_image(blur, "dog-sharpen")

# im = load_image("data/dog.jpg")
# f = make_box_filter(7)
# blur = convolve_image(im, f, 1)
# thumb = nn_resize(blur, blur.w//7, blur.h//7)
# save_image(thumb, "dogthumb")

# im = load_image("data/dog.jpg")
# f = make_gaussian_filter(2)
# blur = convolve_image(im, f, 1)
# save_image(blur, "dog-gauss2")

# im = load_image("data/dog.jpg")
# f = make_gaussian_filter(2)
# lfreq = convolve_image(im, f, 1)
# hfreq = im - lfreq
# reconstruct = lfreq + hfreq
# save_image(lfreq, "low-frequency")
# save_image(hfreq, "high-frequency")
# save_image(reconstruct, "reconstruct")

# im = load_image("data/dog.jpg")
# res = sobel_image(im)
# mag = res[0]
# theta = res[1]
# feature_normalize(mag)
# save_image(mag, "magnitude")
# feature_normalize(theta)
# save_image(theta, "theta")

# im = load_image("data/dog.jpg")
# res = colorize_sobel(im)
# save_image(res, "color-sobel")

im1 = load_image("data/melisa.png")
im2 = load_image("data/aria.png")
f = make_gaussian_filter(2)
im1_lf = convolve_image(im1, f, 1)
im2_lf = convolve_image(im2, f, 1)
im2_hf = sub_image(im2, im2_lf)
im_mix = add_image(im1_lf, im2_hf)
clamp_image(im_mix)
# im_big = bilinear_resize(im_mix, im_mix.w*2, im_mix.h*2)
im_small = bilinear_resize(im_mix, im_mix.w//5, im_mix.h//5)
save_image(im_mix, "melisa-aria")
# save_image(im_big, "melisa-aria_big")
save_image(im_small, "melisa-aria_small")
