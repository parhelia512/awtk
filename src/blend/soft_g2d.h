﻿/**
 * File:   soft_g2d.h
 * Author: AWTK Develop Team
 * Brief:  software implemented image operations
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
 * 2018-05-09 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_SOFT_G2D_H
#define TK_SOFT_G2D_H

#include "tkc/rect.h"
#include "base/bitmap.h"

BEGIN_C_DECLS

ret_t soft_fill_rect(bitmap_t* dst, const rect_t* dst_r, color_t c);
ret_t soft_clear_rect(bitmap_t* dst, const rect_t* dst_r, color_t c);
ret_t soft_copy_image(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, xy_t dx, xy_t dy);
ret_t soft_rotate_image(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, lcd_orientation_t o);
ret_t soft_blend_image(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r, const rectf_t* src_r,
                       uint8_t global_alpha);
#ifdef WITH_FAST_LCD_PORTRAIT
ret_t soft_rotate_image_ex(bitmap_t* dst, bitmap_t* src, const rect_t* src_r, xy_t dx, xy_t dy,
                           lcd_orientation_t o);
ret_t soft_blend_image_rotate(bitmap_t* dst, bitmap_t* src, const rectf_t* dst_r,
                              const rectf_t* src_r, uint8_t alpha, lcd_orientation_t o);
#endif
END_C_DECLS

#endif /*TK_SOFT_G2D_H*/
