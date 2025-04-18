﻿/**
 * File:   canvas_offline.c
 * Author: AWTK Develop Team
 * Brief:  offline canvas.
 *
 * Copyright (c) 2018 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2020-07-10 Luo Zhiming <luozhiming@zlg.cn> created
 *
 */

#include "tkc/wstr.h"
#include "tkc/mem.h"
#include "tkc/utf8.h"
#include "tkc/utils.h"
#include "base/canvas.h"
#include "base/opengl.h"
#include "base/system_info.h"

#include "widget.h"
#include "bitmap.h"
#include "native_window.h"
#include "widget_consts.h"
#include "window_manager.h"
#include "canvas_offline.h"
#include "base/vgcanvas_asset_manager.h"

#ifdef WITH_CANVAS_OFFLINE_CUSTION
#error Do not define WITH_CANVAS_OFFLINE_CUSTION, please define WITH_CANVAS_OFFLINE_CUSTOM !
#endif

#ifdef WITH_GPU

typedef struct _canvas_offline_gpu_t {
  canvas_offline_t base;
  uint8_t* bitmap_data;
  framebuffer_object_t* fbo;
} canvas_offline_gpu_t;

#else

#include "../blend/image_g2d.h"
#include "../lcd/lcd_mem_rgb565.h"
#include "../lcd/lcd_mem_bgr565.h"
#include "../lcd/lcd_mem_rgba8888.h"
#include "../lcd/lcd_mem_bgra8888.h"

#ifdef LINUX
#define WITH_LCD_RGB888 1
#endif/*LINUX*/

#ifdef WITH_LCD_RGB888
#include "../lcd/lcd_mem_rgb888.h"
#include "../lcd/lcd_mem_bgr888.h"
#endif/*WITH_LCD_RGB888*/

#endif

static ret_t canvas_offline_begin_frame(canvas_t* c, const dirty_rects_t* dirty_rects,
                                        lcd_draw_mode_t draw_mode) {
  ret_t ret = RET_OK;
  const rect_t* dirty_rect = dirty_rects != NULL ? &(dirty_rects->max) : NULL;
  ret = canvas_offline_begin_draw(c);
  return_value_if_fail(ret == RET_OK, ret);
  ret = canvas_set_clip_rect(c, dirty_rect);
  return_value_if_fail(ret == RET_OK, ret);
  return RET_OK;
}

canvas_t* canvas_offline_create_by_widget(widget_t* widget, bitmap_format_t format) {
  wh_t w, h;
  canvas_t* canvas = NULL;
  system_info_t* info = system_info();
  return_value_if_fail(widget != NULL, NULL);
  w = widget->w;
  h = widget->h;
#if defined(WITH_FAST_LCD_PORTRAIT)
  if (info->flags & SYSTEM_INFO_FLAG_FAST_LCD_PORTRAIT) {
    if (info->lcd_orientation == LCD_ORIENTATION_90 || info->lcd_orientation == LCD_ORIENTATION_270) {
      w = widget->h;
      h = widget->w;
    }
  }
#else
  (void)info;
#endif
  canvas = canvas_offline_create(w, h, format);
  return_value_if_fail(canvas != NULL, NULL);
  return canvas;
}

canvas_t* canvas_offline_create(uint32_t w, uint32_t h, bitmap_format_t format) {
  canvas_t* c = NULL;
  system_info_t* info = system_info();
  native_window_t* native_window = NULL;

#ifdef WITH_GPU
  ret_t ret = RET_OK;
  vgcanvas_t* vg = NULL;
  framebuffer_object_t* fbo = NULL;
  canvas_offline_gpu_t* canvas = NULL;
#else
  lcd_t* lcd = NULL;
  uint8_t* buff = NULL;
  bitmap_t* bitmap = NULL;
  canvas_offline_t* canvas = NULL;
#endif

#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  canvas_t* custom_canvas = canvas_offline_custom_create(w, h, format);
  if (custom_canvas != NULL) {
    return custom_canvas;
  }
#endif

#ifdef FRAGMENT_FRAME_BUFFER_SIZE
  log_warn(" fragment frame buffer not supported yet\n");
  return NULL;
#endif

  native_window =
      (native_window_t*)widget_get_prop_pointer(window_manager(), WIDGET_PROP_NATIVE_WINDOW);
  return_value_if_fail(native_window != NULL, NULL);

  c = native_window_get_canvas(native_window);
#ifdef WITH_GPU

  if (format != BITMAP_FMT_RGBA8888) {
    assert(!" opengl gpu only supported RGBA foramt ");
    log_warn(" opengl gpu only supported RGBA foramt \n");
    return NULL;
  }

  canvas = TKMEM_ZALLOC(canvas_offline_gpu_t);
  return_value_if_fail(canvas != NULL, NULL);
  canvas->fbo = TKMEM_ZALLOC(framebuffer_object_t);
  if (canvas->fbo == NULL) {
    TKMEM_FREE(canvas);
    canvas = NULL;
  }
  return_value_if_fail(canvas != NULL, NULL);
  fbo = canvas->fbo;

  canvas->base.bitmap = bitmap_create();
  canvas->base.bitmap->w = w * c->lcd->ratio;
  canvas->base.bitmap->h = h * c->lcd->ratio;
  canvas->base.bitmap->should_free_handle = TRUE;
  canvas->base.bitmap->format = BITMAP_FMT_RGBA8888;
  bitmap_set_line_length(canvas->base.bitmap, 0);

  canvas->base.bitmap->buffer = GRAPHIC_BUFFER_CREATE_WITH_DATA(
      NULL, canvas->base.bitmap->w, canvas->base.bitmap->h, BITMAP_FMT_RGBA8888);
  canvas->base.bitmap->should_free_data = TRUE;

  canvas->base.begin_draw = 0;
  canvas->base.lcd_w = c->lcd->w;
  canvas->base.lcd_h = c->lcd->h;
  canvas->base.physical_width = w;
  canvas->base.physical_height = h;

  vg = lcd_get_vgcanvas(c->lcd);
  return_value_if_fail(c != NULL && vg != NULL, NULL);

  canvas_init((canvas_t*)canvas, c->lcd, widget_get_font_manager(window_manager()));
  canvas_set_assets_manager((canvas_t*)canvas, widget_get_assets_manager(window_manager()));
  canvas_set_global_alpha((canvas_t*)canvas, 0xff);
  ret = vgcanvas_create_fbo(vg, w, h, FALSE, fbo);
  if (ret != RET_OK) {
    assert(!" create fbo fail \n");
    log_warn(" create fbo fail \n");
  }
  fbo_to_img(canvas->fbo, canvas->base.bitmap);
#else
  (void)c;
  canvas = TKMEM_ZALLOC(canvas_offline_t);
  return_value_if_fail(canvas != NULL, NULL);

  canvas->begin_draw = 0;
  canvas->bitmap = bitmap_create_ex(w, h, 0, format);

  canvas->physical_width = w;
  canvas->physical_height = h;
  canvas->lcd_w = info->lcd_w;
  canvas->lcd_h = info->lcd_h;

  bitmap = canvas->bitmap;

  buff = bitmap_lock_buffer_for_write(bitmap);
  return_value_if_fail(buff != NULL, NULL);
  if (bitmap->format == BITMAP_FMT_RGBA8888) {
    lcd = lcd_mem_rgba8888_create_single_fb_bitmap(bitmap);
  } else if (bitmap->format == BITMAP_FMT_BGRA8888) {
    lcd = lcd_mem_bgra8888_create_single_fb_bitmap(bitmap);
  } else if (bitmap->format == BITMAP_FMT_BGR565) {
    lcd = lcd_mem_bgr565_create_single_fb_bitmap(bitmap);
  } else if (bitmap->format == BITMAP_FMT_RGB565) {
    lcd = lcd_mem_rgb565_create_single_fb_bitmap(bitmap);
  }
#ifdef WITH_LCD_RGB888
  else if (bitmap->format == BITMAP_FMT_RGB888) {
    lcd = lcd_mem_rgb888_create_single_fb_bitmap(bitmap);
  } else if (bitmap->format == BITMAP_FMT_BGR888) {
    lcd = lcd_mem_bgr888_create_single_fb_bitmap(bitmap);
  }
#endif /*WITH_LCD_RGB888*/

  else {
    assert(!" bitmap format not supported yet \n");
    log_warn(" bitmap format not supported yet \n");
  }
  bitmap_unlock_buffer(bitmap);

  if (lcd != NULL) {
    canvas_t* c_tmp = NULL;

    system_info_set_lcd_w(info, canvas->lcd_w);
    system_info_set_lcd_h(info, canvas->lcd_h);

    c_tmp = canvas_init(&(canvas->base), lcd, widget_get_font_manager(window_manager()));
    canvas_set_assets_manager(c_tmp, widget_get_assets_manager(window_manager()));
    canvas_set_global_alpha(&(canvas->base), 0xff);
  } else {
    TKMEM_FREE(canvas);
    return NULL;
  }
#endif
  canvas_offline_set_canvas_orientation((canvas_t*)canvas, info->lcd_orientation);
  ((canvas_t*)canvas)->end_frame = canvas_offline_end_draw;
  ((canvas_t*)canvas)->begin_frame = canvas_offline_begin_frame;
  return (canvas_t*)canvas;
}

ret_t canvas_offline_set_canvas_orientation(canvas_t* canvas, lcd_orientation_t canvas_orientation) {
  system_info_t* info = system_info();
  canvas_offline_t* canvas_offline = (canvas_offline_t*)canvas;
  return_value_if_fail(canvas_offline != NULL, RET_BAD_PARAMS);
  if (canvas_offline->canvas_orientation != canvas_orientation) {
#if defined(WITH_FAST_LCD_PORTRAIT)
    if (canvas_orientation != LCD_ORIENTATION_0) {
      if (info->flags & SYSTEM_INFO_FLAG_FAST_LCD_PORTRAIT) {
        canvas_offline->bitmap->orientation = canvas_orientation;
        canvas_offline->bitmap->flags |= BITMAP_FLAG_LCD_ORIENTATION;
#ifndef WITH_GPU
        lcd_set_orientation(canvas->lcd, canvas_offline->canvas_orientation, canvas_orientation);
#endif
        if (canvas_orientation == LCD_ORIENTATION_90 ||
            canvas_orientation == LCD_ORIENTATION_270) {
          canvas_offline->bitmap->w = canvas_offline->physical_height;
          canvas_offline->bitmap->h = canvas_offline->physical_width;
        }
      }
    } else {
      if (info->flags & SYSTEM_INFO_FLAG_FAST_LCD_PORTRAIT) {
        canvas_offline->bitmap->orientation = canvas_orientation;
        canvas_offline->bitmap->flags &= ~BITMAP_FLAG_LCD_ORIENTATION;
        lcd_set_orientation(canvas->lcd, canvas_offline->canvas_orientation, LCD_ORIENTATION_0);
        canvas_offline->bitmap->h = canvas_offline->physical_height;
        canvas_offline->bitmap->w = canvas_offline->physical_width;
      }
    }
#else
  (void)info;
#endif
    canvas_offline->canvas_orientation = canvas_orientation;
  }
  return RET_OK;
}

#ifdef AWTK_WEB
ret_t canvas_offline_clear_canvas(canvas_t* canvas) {
#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  if (canvas_offline_custom_clear_canvas(canvas) == RET_OK) {
    return RET_OK;
  }
#endif
  log_warn("canvas_offline_clear_canvas funtion do not supported web ~! \r\n");
  return RET_FAIL;
}
#else
ret_t canvas_offline_clear_canvas(canvas_t* canvas) {
#ifndef WITH_GPU
  rect_t rect;
  canvas_offline_t* canvas_offline = NULL;
#endif
  return_value_if_fail(canvas != NULL, RET_BAD_PARAMS);

#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  if (canvas_offline_custom_clear_canvas(canvas) == RET_OK) {
    return RET_OK;
  }
#endif

#ifdef WITH_GPU
  glEnable(GL_SCISSOR_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glDisable(GL_SCISSOR_TEST);
#else
  canvas_offline = (canvas_offline_t*)canvas;
  rect = rect_init(0, 0, bitmap_get_physical_width(canvas_offline->bitmap), 
                         bitmap_get_physical_height(canvas_offline->bitmap));
  image_clear(canvas_offline->bitmap, &rect, color_init(0x0, 0x0, 0x0, 0x0));
#endif
  return RET_OK;
}
#endif

ret_t canvas_offline_begin_draw(canvas_t* canvas) {
  vgcanvas_t* vg = NULL;
  system_info_t* info = system_info();
  canvas_begin_frame_t begin_frame = NULL;
  canvas_offline_t* canvas_offline = NULL;
#ifdef WITH_GPU
  canvas_offline_gpu_t* c = NULL;
#endif

#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  ret_t ret = canvas_offline_custom_begin_draw(canvas);
  if (ret == RET_OK) {
    return ret;
  }
#endif

  return_value_if_fail(canvas != NULL, RET_BAD_PARAMS);
  begin_frame = canvas->begin_frame;
  canvas->begin_frame = NULL;

  vg = lcd_get_vgcanvas(canvas->lcd);
  canvas_offline = (canvas_offline_t*)canvas;
#ifdef WITH_GPU
  c = (canvas_offline_gpu_t*)canvas;
  if (vg != NULL && canvas_offline->begin_draw == 0) {
    canvas_get_clip_rect(canvas, &canvas_offline->canvas_clip_rect);
    c->base.lcd_w = canvas->lcd->w;
    c->base.lcd_h = canvas->lcd->h;

    vgcanvas_flush(vg);

    canvas_offline->system_lcd_orientation = info->lcd_orientation;
    lcd_set_orientation(canvas->lcd, canvas_offline->system_lcd_orientation, info->lcd_orientation);
    system_info_set_lcd_orientation(info, canvas_offline->canvas_orientation);
    vgcanvas_bind_fbo(vg, c->fbo);
    vgcanvas_save(vg);

#if !defined(WITH_FAST_LCD_PORTRAIT)
    canvas->lcd->w = vg->w = c->fbo->w;
    canvas->lcd->h = vg->h = c->fbo->h;
#endif
    canvas_set_clip_rect(canvas, NULL);
  }
#else
  if (canvas_offline->begin_draw == 0) {
    canvas_offline->lcd_w = info->lcd_w;
    canvas_offline->lcd_h = info->lcd_h;
    canvas_offline->system_lcd_orientation = info->lcd_orientation;
    system_info_set_lcd_orientation(info, canvas_offline->canvas_orientation);
    system_info_set_lcd_w(info, lcd_get_physical_width(canvas_offline->base.lcd));
    system_info_set_lcd_h(info, lcd_get_physical_height(canvas_offline->base.lcd));

    canvas_begin_frame(canvas, NULL, LCD_DRAW_OFFLINE);
    canvas_get_clip_rect(canvas, &canvas_offline->canvas_clip_rect);
    if (vg != NULL) {
      vgcanvas_begin_frame(vg, NULL);
      vgcanvas_save(vg);
    }
    canvas_set_clip_rect(canvas, NULL);
  }
#endif
  canvas_offline->begin_draw++;
  canvas->begin_frame = begin_frame;
  return RET_OK;
}

ret_t canvas_offline_end_draw(canvas_t* canvas) {
  vgcanvas_t* vg = NULL;
  system_info_t* info = system_info();
  canvas_end_frame_t end_frame = NULL;
  canvas_offline_t* canvas_offline = NULL;
#ifdef WITH_GPU
  canvas_offline_gpu_t* c = NULL;
#endif

#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  ret_t ret = canvas_offline_custom_end_draw(canvas);
  if (ret == RET_OK) {
    return ret;
  }
#endif

  return_value_if_fail(canvas != NULL, RET_BAD_PARAMS);
  end_frame = canvas->end_frame;
  canvas->end_frame = NULL;

  vg = lcd_get_vgcanvas(canvas->lcd);
  canvas_offline = (canvas_offline_t*)canvas;

  canvas_offline->begin_draw--;
#ifdef WITH_GPU
  c = (canvas_offline_gpu_t*)canvas;
  if (vg != NULL && canvas_offline->begin_draw == 0) {
    canvas->lcd->w = vg->w = c->base.lcd_w;
    canvas->lcd->h = vg->h = c->base.lcd_h;

    vgcanvas_restore(vg);
    lcd_set_orientation(canvas->lcd, info->lcd_orientation, canvas_offline->system_lcd_orientation);
    system_info_set_lcd_orientation(info, canvas_offline->system_lcd_orientation);
    vgcanvas_unbind_fbo(vg, c->fbo);
    canvas_set_clip_rect(canvas, &canvas_offline->canvas_clip_rect);
  }
#else
  if (canvas_offline->begin_draw == 0) {
    system_info_set_lcd_w(info, canvas_offline->lcd_w);
    system_info_set_lcd_h(info, canvas_offline->lcd_h);
    system_info_set_lcd_orientation(info, canvas_offline->system_lcd_orientation);

    canvas_set_clip_rect(canvas, &canvas_offline->canvas_clip_rect);
    if (vg != NULL) {
      vgcanvas_restore(vg);
      vgcanvas_end_frame(vg);
    }
  }
#endif
  canvas->end_frame = end_frame;
  return RET_OK;
}

bitmap_t* canvas_offline_get_bitmap(canvas_t* canvas) {
  canvas_offline_t* c = (canvas_offline_t*)canvas;

#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  bitmap_t* bitmap = canvas_offline_custom_get_bitmap(canvas);
  if (bitmap != NULL) {
    return bitmap;
  }
#endif

  return_value_if_fail(canvas != NULL && c != NULL, NULL);
  return c->bitmap;
}
#ifdef AWTK_WEB
typedef struct _web_bitmap_ctx_t {
  vgcanvas_t* vg;
  framebuffer_object_t* fbo;
} web_bitmap_ctx_t;

static ret_t canvas_offline_web_bitmap_destroy(bitmap_t* img) {
  web_bitmap_ctx_t* ctx = (web_bitmap_ctx_t*)(img->specific_ctx);
  vgcanvas_destroy_fbo(ctx->vg, ctx->fbo);
  TKMEM_FREE(ctx->fbo);
  TKMEM_FREE(ctx);
  return RET_OK;
}
#endif

ret_t canvas_offline_bitmap_move_to_new_bitmap(canvas_t* canvas, bitmap_t* bitmap) {
  bool_t should_free_handle;
  bitmap_t* canvas_bitmap = NULL;

#ifdef AWTK_WEB
  web_bitmap_ctx_t* ctx = TKMEM_ZALLOC(web_bitmap_ctx_t);
  return_value_if_fail(ctx != NULL, RET_OOM);
#endif

  return_value_if_fail(canvas != NULL && bitmap != NULL, RET_BAD_PARAMS);

#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  if (canvas_offline_custom_bitmap_move_to_new_bitmap(canvas, bitmap) == RET_OK) {
    return RET_OK;
  }
#endif

  should_free_handle = bitmap->should_free_handle;
  canvas_bitmap = ((canvas_offline_t*)canvas)->bitmap;

  memcpy(bitmap, canvas_bitmap, sizeof(bitmap_t));

  bitmap->should_free_handle = should_free_handle;

#ifdef AWTK_WEB
  ctx->vg = canvas_get_vgcanvas(canvas);
  ctx->fbo = ((canvas_offline_gpu_t*)canvas)->fbo;
  ((canvas_offline_gpu_t*)canvas)->fbo = NULL;
  vgcanvas_asset_manager_remove_image(vgcanvas_asset_manager(), lcd_get_vgcanvas(canvas->lcd),
                                      canvas_bitmap);

  bitmap->specific_ctx = ctx;
  bitmap->image_manager = image_manager();
  bitmap->specific_destroy = canvas_offline_web_bitmap_destroy;
#endif
  if (bitmap_flag_is_lcd_orientation(canvas_bitmap)) {
    bitmap->flags |= BITMAP_FLAG_LCD_ORIENTATION;
  }
  bitmap->flags |= BITMAP_FLAG_IMMUTABLE;
  ((canvas_offline_t*)canvas)->bitmap = NULL;

  TKMEM_FREE(canvas_bitmap);
  return RET_OK;
}

ret_t canvas_offline_flush_bitmap(canvas_t* canvas) {
#ifdef WITH_GPU
  rect_t r;
  vgcanvas_t* vg = NULL;
  canvas_offline_gpu_t* c = (canvas_offline_gpu_t*)canvas;
#endif
  return_value_if_fail(canvas != NULL, RET_BAD_PARAMS);

#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  if (canvas_offline_custom_flush_bitmap(canvas) == RET_OK) {
    return RET_OK;
  }
#endif

#ifdef WITH_GPU
  vg = lcd_get_vgcanvas(canvas->lcd);
  if (vg != NULL && c->fbo != NULL) {
    uint8_t* data = NULL;
    bitmap_t* bitmap = c->base.bitmap;
    framebuffer_object_t* fbo = c->fbo;
    if (c->bitmap_data == NULL) {
      data = TKMEM_ALLOC(bitmap->line_length * bitmap->h);
      return_value_if_fail(data != NULL, RET_OOM);
      c->bitmap_data = data;
    } else {
      data = c->bitmap_data;
    }
    r = rect_init(0, 0, bitmap->w, bitmap->h);

    graphic_buffer_attach(bitmap->buffer, data, bitmap_get_physical_width(bitmap), bitmap_get_physical_height(bitmap));
    vgcanvas_fbo_to_bitmap(vg, fbo, bitmap, &r);
  }
#endif
  return RET_OK;
}

ret_t canvas_offline_destroy(canvas_t* canvas) {
  bitmap_t* bitmap = NULL;
#ifdef WITH_GPU
  vgcanvas_t* vg = NULL;
  canvas_offline_gpu_t* canvas_offline_gpu = NULL;
#else
  system_info_t* info = system_info();
#endif

  return_value_if_fail(canvas != NULL, RET_BAD_PARAMS);

#ifdef WITH_CANVAS_OFFLINE_CUSTOM
  if (canvas_offline_custom_destroy(canvas) == RET_OK) {
    return RET_OK;
  }
#endif

#ifdef WITH_GPU
  vg = lcd_get_vgcanvas(canvas->lcd);
  if (vg != NULL) {
    /* 在 OpenGL 模式下，防止用户在调用完 draw_image 后，马上释放离线画布导致离线画布的位图数据被释放掉，导致绘图异常 */
    vgcanvas_flush(vg);
  }
#endif

  bitmap = ((canvas_offline_t*)canvas)->bitmap;
  if (bitmap != NULL) {
    bitmap_destroy(bitmap);
  }
#ifdef WITH_GPU
  canvas_offline_gpu = (canvas_offline_gpu_t*)canvas;
  if (canvas_offline_gpu->bitmap_data != NULL) {
    TKMEM_FREE(canvas_offline_gpu->bitmap_data);
  }
  canvas_reset(canvas);
#else

  if (canvas->lcd != NULL) {
    lcd_t* lcd = canvas->lcd;
    canvas_reset(canvas);
    lcd_destroy(lcd);
  } else {
    canvas_reset(canvas);
  }
#endif

  TKMEM_FREE(canvas);

  return RET_OK;
}
