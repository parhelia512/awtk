﻿/**
 * File:   window_animator_common.c
 * Author: AWTK Develop Team
 * Brief:  window animator common used functions.
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
 * 2019-03-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifdef AWTK_WEB
#include <emscripten.h>
#endif /*AWTK_WEB*/

#include "base/window_manager.h"
#include "base/window_animator.h"
#include "base/dialog_highlighter_factory.h"
#include "window_manager/window_manager_default.h"
#include "window_animators/window_animator_common.h"

ret_t window_animator_to_bottom_draw_curr(window_animator_t* wa) {
  canvas_t* c = wa->canvas;
  widget_t* win = wa->curr_win;
  float_t percent = wa->percent;
#ifndef WITHOUT_WINDOW_ANIMATOR_CACHE
  int32_t y = win->y;
  int32_t h = tk_roundi(win->h * percent);

  rectf_t src = rectf_init(win->x, win->y + win->h - h, win->w, h);
  rectf_t dst = rectf_init(win->x, y, win->w, h);

  if (h == 0) {
    return RET_OK;
  }

  return lcd_draw_image(c->lcd, &(wa->curr_img), rectf_scale(&src, wa->ratio), &dst);
#else
  int32_t y = -win->h * (1 - percent);
  canvas_translate(c, 0, y);
  widget_paint(win, c);
  canvas_untranslate(c, 0, y);
  return RET_OK;
#endif /*WITHOUT_WINDOW_ANIMATOR_CACHE*/
}

ret_t window_animator_to_top_draw_curr(window_animator_t* wa) {
  canvas_t* c = wa->canvas;
  widget_t* win = wa->curr_win;
  float_t percent = wa->percent;
#ifndef WITHOUT_WINDOW_ANIMATOR_CACHE
  int32_t y = win->y + tk_roundi(win->h * (1 - percent));
  int32_t h = win->y + win->h - y;

  rectf_t src = rectf_init(win->x, win->y, win->w, h);
  rectf_t dst = rectf_init(win->x, y, win->w, h);

  if (h == 0) {
    return RET_OK;
  }

  return lcd_draw_image(c->lcd, &(wa->curr_img), rectf_scale(&src, wa->ratio), &dst);
#else
  int32_t y = win->h * (1 - percent);
  canvas_translate(c, 0, y);
  widget_paint(win, c);
  canvas_untranslate(c, 0, y);
  return RET_OK;
#endif /*WITHOUT_WINDOW_ANIMATOR_CACHE*/
}

ret_t window_animator_to_left_draw_curr(window_animator_t* wa) {
  canvas_t* c = wa->canvas;
  widget_t* win = wa->curr_win;
  float_t percent = wa->percent;
#ifndef WITHOUT_WINDOW_ANIMATOR_CACHE
  int32_t x = win->x + tk_roundi(win->w * (1 - percent));
  int32_t w = win->x + win->w - x;

  rectf_t src = rectf_init(win->x, win->y, w, win->h);
  rectf_t dst = rectf_init(x, win->y, w, win->h);

  if (w == 0) {
    return RET_OK;
  }

  return lcd_draw_image(c->lcd, &(wa->curr_img), rectf_scale(&src, wa->ratio), &dst);
#else
  int32_t x = -win->w * (1 - percent);

  canvas_translate(c, x, 0);
  widget_paint(win, c);
  canvas_untranslate(c, x, 0);
  return RET_OK;
#endif /*WITHOUT_WINDOW_ANIMATOR_CACHE*/
}

ret_t window_animator_to_right_draw_curr(window_animator_t* wa) {
  canvas_t* c = wa->canvas;
  widget_t* win = wa->curr_win;
  float_t percent = wa->percent;
#ifndef WITHOUT_WINDOW_ANIMATOR_CACHE
  int32_t x = win->x;
  int32_t w = tk_roundi(win->w * percent);
  rectf_t src = rectf_init(win->x + win->w - w, win->y, w, win->h);
  rectf_t dst = rectf_init(x, win->y, w, win->h);

  if (w == 0) {
    return RET_OK;
  }

  return lcd_draw_image(c->lcd, &(wa->curr_img), rectf_scale(&src, wa->ratio), &dst);
#else
  int32_t x = win->w * (1 - percent);

  canvas_translate(c, x, 0);
  widget_paint(win, c);
  canvas_untranslate(c, x, 0);
  return RET_OK;
#endif /*WITHOUT_WINDOW_ANIMATOR_CACHE*/
}

static ret_t window_animator_update_percent(window_animator_t* wa);
static ret_t window_animator_draw_prev_window(window_animator_t* wa);
static ret_t window_animator_draw_curr_window(window_animator_t* wa);

static ret_t window_animator_open_destroy(window_animator_t* wa) {
#ifndef WITHOUT_WINDOW_ANIMATOR_CACHE
  if (wa->dialog_highlighter == NULL) {
    bitmap_destroy(&(wa->prev_img));
  }
  bitmap_destroy(&(wa->curr_img));
#endif /*WITHOUT_WINDOW_ANIMATOR_CACHE*/

  memset(wa, 0x00, sizeof(window_animator_t));
  TKMEM_FREE(wa);

  return RET_OK;
}

static ret_t window_animator_close_destroy(window_animator_t* wa) {
  widget_invalidate_force(wa->prev_win, NULL);
  widget_destroy(wa->curr_win);

  return window_animator_open_destroy(wa);
}

ret_t window_animator_update(window_animator_t* wa, uint64_t time_ms) {
  return_value_if_fail(wa != NULL, RET_FAIL);

  if (wa->start_time == 0) {
    wa->start_time = time_ms;
  }

  wa->time_percent = (time_ms - wa->start_time) / (float_t)(wa->duration);
  if (wa->time_percent >= 1) {
    wa->time_percent = 1;
  }

  ENSURE(window_animator_update_percent(wa) == RET_OK);
  ENSURE(window_animator_draw_prev_window(wa) == RET_OK);
  ENSURE(window_animator_draw_curr_window(wa) == RET_OK);

  return wa->time_percent >= 1 ? RET_DONE : RET_OK;
}

ret_t window_animator_destroy(window_animator_t* wa) {
  return_value_if_fail(wa != NULL, RET_FAIL);

  if (wa->open) {
    return window_animator_open_destroy(wa);
  } else {
    return window_animator_close_destroy(wa);
  }
}

/******************helper******************/

#ifndef WITHOUT_WINDOW_ANIMATORS
static ret_t window_animator_init(window_animator_t* wa) {
  ret_t ret = RET_NOT_IMPL;
  return_value_if_fail(wa != NULL && wa->vt != NULL, RET_BAD_PARAMS);

  if (wa->vt->init != NULL) {
    ret = wa->vt->init(wa);
  }

  return ret;
}

ret_t window_animator_prepare(window_animator_t* wa, canvas_t* c, widget_t* prev_win,
                              widget_t* curr_win) {
  widget_t* wm = prev_win->parent;

  wa->canvas = c;
  wa->prev_win = prev_win;
  wa->curr_win = curr_win;
  wa->ratio = c->lcd->ratio;
  wa->duration = wa->duration ? wa->duration : 500;

  window_animator_init(wa);
#ifndef WITHOUT_WINDOW_ANIMATOR_CACHE
  window_manager_snap_prev_window(wm, prev_win, &(wa->prev_img));
  window_manager_snap_curr_window(wm, curr_win, &(wa->curr_img));
  wa->dialog_highlighter = window_manager_get_dialog_highlighter(wm);
#endif /*WITHOUT_WINDOW_ANIMATOR_CACHE*/

  return RET_OK;
}
#else
ret_t window_animator_prepare(window_animator_t* wa, canvas_t* c, widget_t* prev_win,
                              widget_t* curr_win) {
  return RET_OK;
}
#endif /*WITHOUT_WINDOW_ANIMATORS*/

window_animator_t* window_animator_create(bool_t open, const window_animator_vtable_t* vt) {
  window_animator_t* wa = NULL;
  return_value_if_fail(vt != NULL && vt->size >= sizeof(window_animator_t), NULL);

  wa = (window_animator_t*)TKMEM_ALLOC(vt->size);
  return_value_if_fail(wa != NULL, NULL);

  memset(wa, 0, vt->size);

  wa->vt = vt;
  wa->open = open;
  wa->easing = easing_get(EASING_CUBIC_OUT);

  return wa;
}

static ret_t window_animator_update_percent_default(window_animator_t* wa) {
  if (wa->open) {
    wa->percent = wa->easing(wa->time_percent);
  } else {
    wa->percent = 1.0f - wa->easing(wa->time_percent);
  }

  return RET_OK;
}

static ret_t window_animator_update_percent(window_animator_t* wa) {
  ret_t ret = RET_OK;
  return_value_if_fail(wa != NULL && wa->vt != NULL, RET_BAD_PARAMS);

  if (wa->vt->update_percent != NULL) {
    ret = wa->vt->update_percent(wa);
  } else {
    ret = window_animator_update_percent_default(wa);
  }
  if (wa->percent >= 1) {
    wa->percent = 1.0f;
  } else if (wa->percent < 0) {
    wa->percent = 0.0f;
  }
  return ret;
}

static ret_t window_animator_draw_prev_window(window_animator_t* wa) {
  return_value_if_fail(wa != NULL && wa->vt != NULL && wa->vt->draw_prev_window, RET_BAD_PARAMS);

  if (wa->dialog_highlighter != NULL) {
    ret_t ret;
    float_t percent = wa->percent;
    /*always < 1 to tell highlighter that it is animating.*/
    if (percent >= 1) {
      percent = 0.999;
    }
    ret = dialog_highlighter_draw(wa->dialog_highlighter, percent);
    if (ret == RET_OK && wa->vt->draw_prev_window_on_highlighter != NULL) {
      ret = wa->vt->draw_prev_window_on_highlighter(wa);
    }
    return ret;
  } else {
    return wa->vt->draw_prev_window(wa);
  }
}

static ret_t window_animator_draw_curr_window(window_animator_t* wa) {
  return_value_if_fail(wa != NULL && wa->vt != NULL && wa->vt->draw_curr_window, RET_BAD_PARAMS);

  return wa->vt->draw_curr_window(wa);
}

ret_t window_animator_overlap_default_draw_prev(window_animator_t* wa) {
  bool_t start = FALSE;
  canvas_t* c = wa->canvas;
  widget_t* wm = window_manager();
#ifndef WITHOUT_WINDOW_ANIMATOR_CACHE
  widget_t* win = wa->prev_win;
  rectf_t src = rectf_init(win->x, win->y, win->w, win->h);
  rectf_t dst = rectf_init(win->x, win->y, win->w, win->h);
  lcd_draw_image(c->lcd, &(wa->prev_img), rectf_scale(&src, wa->ratio), &dst);
  WIDGET_FOR_EACH_CHILD_BEGIN(wm, iter, i)
  if (iter == wa->curr_win) {
    break;
  }
  if (iter == wa->prev_win) {
    start = TRUE;
  }
  if (start) {
    /* 非普通窗口应该需要重绘，因为可能会叠在 system_bar 上面 */
    src = rectf_init(iter->x, iter->y, iter->w, iter->h);
    dst = rectf_init(iter->x, iter->y, iter->w, iter->h);
    lcd_draw_image(c->lcd, &(wa->prev_img), rectf_scale(&src, wa->ratio), &dst);
  }
  WIDGET_FOR_EACH_CHILD_END()
#else
  WIDGET_FOR_EACH_CHILD_BEGIN(wm, iter, i)
  if (iter == wa->curr_win) {
    break;
  }
  if (iter == wa->prev_win) {
    start = TRUE;
  }
  if (start) {
    widget_paint(iter, c);
  }
  WIDGET_FOR_EACH_CHILD_END()
#endif /*WITHOUT_WINDOW_ANIMATOR_CACHE*/
  return RET_OK;
}

ret_t window_animator_begin_frame(window_animator_t* wa) {
  return_value_if_fail(wa != NULL && wa->vt != NULL, RET_OK);

  ENSURE(canvas_begin_frame(wa->canvas, NULL, LCD_DRAW_ANIMATION) == RET_OK);
  /* 把 system_bar 绘制到底部，然后再被截图覆盖在上面，应该截图中的对话框等窗口有可能叠在 system_bar 上面 */
  if (wa->is_paint_system_bar_top) {
    window_manager_paint_system_bar_top(wa->curr_win->parent, wa->canvas);
  }
  if (wa->is_paint_system_bar_bottom) {
    window_manager_paint_system_bar_bottom(wa->curr_win->parent, wa->canvas);
  }

  return RET_OK;
}

ret_t window_animator_end_frame(window_animator_t* wa) {
  return_value_if_fail(wa != NULL && wa->vt != NULL, RET_OK);
  return canvas_end_frame(wa->canvas);
}
