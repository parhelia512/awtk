﻿/**
 * File:   fill_image_bgr888.c
 * Author: AWTK Develop Team
 * Brief:  fill on bgr888
 *
 * Copyright (c) 2018 - 2025 Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2022-02-24 Generated by gen.sh(DONT MODIFY IT)
 *
 */
#include "tkc/rect.h"
#include "base/pixel.h"
#include "base/bitmap.h"
#include "base/pixel_pack_unpack.h"

#define pixel_dst_t pixel_bgr888_t
#define pixel_dst_format pixel_bgr888_format
#define pixel_dst_to_rgba pixel_bgr888_to_rgba
#define pixel_dst_from_rgb pixel_bgr888_from_rgb
#define pixel_dst_from_rgba pixel_bgr888_from_rgba

#define pixel_t pixel_dst_t
#define pixel_from_rgb pixel_dst_from_rgb
#define pixel_from_rgba pixel_dst_from_rgba
#define pixel_to_rgba pixel_dst_to_rgba

#define pixel_blend_rgba_dark pixel_bgr888_blend_rgba_dark
#define pixel_blend_rgba_premulti pixel_bgr888_blend_rgba_premulti

#include "pixel_ops.inc"
#include "fill_image.inc"

ret_t fill_bgr888_rect(bitmap_t* fb, const rect_t* dst, color_t c) {
  return fill_image(fb, dst, c);
}

ret_t clear_bgr888_rect(bitmap_t* fb, const rect_t* dst, color_t c) {
  return clear_image(fb, dst, c);
}
