﻿/**
 * File:   progress_circle.c
 * Author: AWTK Develop Team
 * Brief:  progress_circle
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
 * 2018-11-10 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "base/widget_vtable.h"
#include "base/image_manager.h"
#include "progress_circle/progress_circle.h"

static float_t progress_circle_get_radius(widget_t* widget);
static ret_t progress_circle_on_paint_background(widget_t* widget, canvas_t* c) {
  float_t r = 0;
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  r = progress_circle_get_radius(widget);
  return widget_draw_arc_at_center(widget, c, TRUE, progress_circle->line_width, 0, M_PI * 2,
                                   progress_circle->counter_clock_wise, progress_circle->line_cap,
                                   r);
}

static ret_t progress_circle_update_text(widget_t* widget) {
  const char* unit = NULL;
  const char* format = NULL;
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  unit = widget_get_prop_str(widget, PROGRESS_CIRCLE_PROP_UNIT, NULL);
  format = progress_circle->format ? progress_circle->format : "%d";
  widget_set_text_with_double(widget, format, progress_circle->value);
  if (unit != NULL) {
    wstr_append_utf8(&(widget->text), unit);
  }

  return RET_OK;
}

static float_t progress_circle_value_to_angle(widget_t* widget, float_t value) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, 0);

  return tk_value_to_angle(value, 0, progress_circle->max, progress_circle->start_angle,
                           progress_circle->start_angle + 360, progress_circle->counter_clock_wise);
}

static float_t progress_circle_get_radius(widget_t* widget) {
  xy_t cx = widget->w / 2;
  xy_t cy = widget->h / 2;
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  ENSURE(progress_circle);

  return tk_min(cx, cy) - progress_circle->line_width / 2;
}

rect_t progress_circle_calc_text_dirty_rect(widget_t* widget) {
  rect_t r = {0, 0, 0, 0};
  canvas_t* c = widget_get_canvas(widget);
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  ENSURE(progress_circle);

  if (widget->w < 1 || widget->h < 1) {
    return r;
  }

  if (c != NULL && progress_circle->show_text) {
    wstr_t* text = &(widget->text);
    widget_prepare_text_style(widget, c);
    progress_circle_update_text(widget);

    r.w = canvas_measure_text(c, text->str, text->size) + 10;
    r.h = c->font_size + 4;

    r.x = (widget->w - r.w) / 2;
    r.y = (widget->h - r.h) / 2;
  }

  return r;
}

rect_t progress_circle_calc_line_dirty_rect(widget_t* widget, float_t old_value,
                                            float_t new_value) {
  int32_t min_x = 0;
  int32_t min_y = 0;
  int32_t max_x = 0;
  int32_t max_y = 0;
  float_t start_angle = 0;
  float_t end_angle = 0;
  rect_t rect = {0, 0, 0, 0};
  point_t start_p = {0, 0};
  point_t end_p = {0, 0};
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  ENSURE(progress_circle);
  float_t line_width = progress_circle->line_width;

  start_angle = progress_circle_value_to_angle(widget, old_value);
  end_angle = progress_circle_value_to_angle(widget, new_value);

  if (widget->w < 1 || widget->h < 1) {
    return rect;
  }

  if (start_angle > end_angle) {
    float_t t = start_angle;
    start_angle = end_angle;
    end_angle = t;
  }

  if (!progress_circle->is_redraw && (end_angle - start_angle) < (M_PI / 2)) {
    xy_t cx = widget->w / 2;
    xy_t cy = widget->h / 2;
    float_t r = progress_circle_get_radius(widget);

    start_p.y = tk_roundi(r * sin(start_angle));
    start_p.x = tk_roundi(r * cos(start_angle));

    end_p.y = tk_roundi(r * sin(end_angle));
    end_p.x = tk_roundi(r * cos(end_angle));

    min_x = tk_min(start_p.x, end_p.x) - line_width;
    max_x = tk_max(start_p.x, end_p.x) + line_width;
    min_y = tk_min(start_p.y, end_p.y) - line_width;
    max_y = tk_max(start_p.y, end_p.y) + line_width;
    if (start_p.x > 0 && end_p.x < 0) {
      /*跨越第1和2象限*/
      max_y = tk_max_int(r, max_y);
    } else if (start_p.y > 0 && end_p.y < 0) {
      /*跨越第2和3象限*/
      min_x = tk_min_int(-r, min_x);
    } else if (start_p.x < 0 && end_p.x > 0) {
      /*跨越第3和4象限*/
      min_y = tk_min_int(-r, min_y);
    } else if (start_p.y < 0 && end_p.y > 0) {
      /*跨越第4和1象限*/
      max_x = tk_max_int(r, max_x);
    }

    assert(min_x <= max_x);
    assert(min_y <= max_y);
    rect = rect_init(min_x, min_y, max_x - min_x, max_y - min_y);
    rect.x += cx;
    rect.y += cy;
  } else {
    progress_circle->is_redraw = FALSE;
    rect = rect_init(0, 0, widget->w, widget->h);
  }

  return rect;
}

static ret_t progress_circle_on_paint_self(widget_t* widget, canvas_t* c) {
  bitmap_t img;
  style_t* style = widget->astyle;
  color_t trans = color_init(0, 0, 0, 0);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  color_t color = style_get_color(style, STYLE_ID_FG_COLOR, trans);
  const char* image_name = style_get_str(style, STYLE_ID_FG_IMAGE, NULL);
  bool_t has_image = image_name && widget_load_image(widget, image_name, &img) == RET_OK;

  if (progress_circle->value > progress_circle->max) {
    progress_circle->value = progress_circle->max;
  }

  if (vg != NULL && (has_image || color.rgba.a)) {
    xy_t cx = widget->w / 2;
    xy_t cy = widget->h / 2;
    float_t end_angle = 0.0f;
    float_t r = progress_circle_get_radius(widget);
    bool_t ccw = progress_circle->counter_clock_wise;
    float_t start_angle = TK_D2R(progress_circle->start_angle);

    if (tk_fequal(progress_circle->value, 0)) {
      end_angle = start_angle;
    } else {
      end_angle = progress_circle_value_to_angle(widget, progress_circle->value);
      if (progress_circle->value == progress_circle->max) {
        end_angle = start_angle + M_PI * 2;
      }
    }

    vgcanvas_save(vg);
    vgcanvas_translate(vg, c->ox, c->oy);
    vgcanvas_set_stroke_color(vg, color);
    vgcanvas_set_line_width(vg, progress_circle->line_width);
    if (tk_str_eq(progress_circle->line_cap, VGCANVAS_LINE_CAP_ROUND)) {
      vgcanvas_set_line_cap(vg, VGCANVAS_LINE_CAP_ROUND);
    } else if (tk_str_eq(progress_circle->line_cap, VGCANVAS_LINE_CAP_SQUARE)) {
      vgcanvas_set_line_cap(vg, VGCANVAS_LINE_CAP_SQUARE);
    } else {
      vgcanvas_set_line_cap(vg, VGCANVAS_LINE_CAP_BUTT);
    }
    vgcanvas_begin_path(vg);
    vgcanvas_set_fill_mode(vg, VGCANVAS_FILL_MODE_NON_ZERO);

    if (end_angle > start_angle) {
      float max_r = tk_min(widget->w, widget->h) / 2;
      if (progress_circle->line_width > max_r / 2) {
        //画扇形
        vgcanvas_set_fill_color(vg, color);
        vgcanvas_move_to(vg, cx, cy);
        vgcanvas_line_to(vg, cx + max_r * cos(start_angle), cy + max_r * sin(start_angle));
        vgcanvas_arc(vg, cx, cy, max_r, start_angle, end_angle, ccw);
        vgcanvas_close_path(vg);

        if (has_image) {
          vgcanvas_paint(vg, FALSE, &img);
        } else {
          vgcanvas_fill(vg);
        }
      } else {
        vgcanvas_arc(vg, cx, cy, r, start_angle, end_angle, ccw);
        if (has_image) {
          vgcanvas_paint(vg, TRUE, &img);
        } else {
          vgcanvas_stroke(vg);
        }
      }
    }

    vgcanvas_restore(vg);
    progress_circle->last_dirty_rect =
        rect_init(progress_circle->dirty_rect.x, progress_circle->dirty_rect.y,
                  progress_circle->dirty_rect.w, progress_circle->dirty_rect.h);
    progress_circle->dirty_rect = rect_init(0, 0, 0, 0);
  }

  color = style_get_color(style, STYLE_ID_TEXT_COLOR, trans);
  if (progress_circle->show_text && color.rgba.a) {
    progress_circle_update_text(widget);
    widget_paint_helper(widget, c, NULL, NULL);
  }

  return RET_OK;
}

ret_t progress_circle_set_value(widget_t* widget, float_t value) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  if (progress_circle->value != value) {
    float_t old_value = progress_circle->value;
    value_change_event_t evt;
    value_change_event_init(&evt, EVT_VALUE_WILL_CHANGE, widget);
    value_set_float(&(evt.old_value), progress_circle->value);
    value_set_float(&(evt.new_value), value);

    if (widget_dispatch(widget, (event_t*)&evt) != RET_STOP) {
      rect_t r = progress_circle_calc_line_dirty_rect(widget, old_value, value);
      rect_merge(&progress_circle->dirty_rect, &r);
      rect_merge(&r, &progress_circle->last_dirty_rect);

      widget_invalidate_force(widget, &r);

      progress_circle->value = value;
      evt.e.type = EVT_VALUE_CHANGED;
      widget_dispatch(widget, (event_t*)&evt);

      if (progress_circle->show_text) {
        r = progress_circle_calc_text_dirty_rect(widget);
        widget_invalidate_force(widget, &r);
      }
    }
  }

  return RET_OK;
}

ret_t progress_circle_set_max(widget_t* widget, uint32_t max) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  progress_circle->max = max;

  return widget_invalidate(widget, NULL);
}

ret_t progress_circle_set_line_width(widget_t* widget, uint32_t line_width) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  progress_circle->line_width = line_width;

  return widget_invalidate(widget, NULL);
}

ret_t progress_circle_set_start_angle(widget_t* widget, int32_t start_angle) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  progress_circle->start_angle = start_angle;

  return widget_invalidate(widget, NULL);
}

ret_t progress_circle_set_line_cap(widget_t* widget, const char* line_cap) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  progress_circle->line_cap = tk_str_copy(progress_circle->line_cap, line_cap);

  return widget_invalidate(widget, NULL);
}

ret_t progress_circle_set_show_text(widget_t* widget, bool_t show_text) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  progress_circle->show_text = show_text;

  return widget_invalidate(widget, NULL);
}

ret_t progress_circle_set_counter_clock_wise(widget_t* widget, bool_t counter_clock_wise) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  progress_circle->counter_clock_wise = counter_clock_wise;

  return widget_invalidate(widget, NULL);
}

static ret_t progress_circle_on_destroy(widget_t* widget) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(widget != NULL && progress_circle != NULL, RET_BAD_PARAMS);

  TKMEM_FREE(progress_circle->format);
  TKMEM_FREE(progress_circle->line_cap);

  return RET_OK;
}

static ret_t progress_circle_get_prop(widget_t* widget, const char* name, value_t* v) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    value_set_float(v, progress_circle->value);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    value_set_uint32(v, progress_circle->max);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_FORMAT)) {
    value_set_str(v, progress_circle->format);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_SHOW_TEXT)) {
    value_set_bool(v, progress_circle->show_text);
    return RET_OK;
  } else if (tk_str_eq(name, PROGRESS_CIRCLE_PROP_LINE_CAP)) {
    value_set_str(v, progress_circle->line_cap);
    return RET_OK;
  } else if (tk_str_eq(name, PROGRESS_CIRCLE_PROP_LINE_WIDTH)) {
    value_set_uint32(v, progress_circle->line_width);
    return RET_OK;
  } else if (tk_str_eq(name, PROGRESS_CIRCLE_PROP_START_ANGLE)) {
    value_set_int(v, progress_circle->start_angle);
    return RET_OK;
  } else if (tk_str_eq(name, PROGRESS_CIRCLE_PROP_COUNTER_CLOCK_WISE)) {
    value_set_bool(v, progress_circle->counter_clock_wise);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t progress_circle_set_prop(widget_t* widget, const char* name, const value_t* v) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    return progress_circle_set_value(widget, value_float(v));
  } else if (tk_str_eq(name, WIDGET_PROP_MAX)) {
    progress_circle->is_redraw = TRUE;
    return progress_circle_set_max(widget, value_int(v));
  } else if (tk_str_eq(name, WIDGET_PROP_FORMAT)) {
    return progress_circle_set_format(widget, value_str(v));
  } else if (tk_str_eq(name, WIDGET_PROP_SHOW_TEXT)) {
    return progress_circle_set_show_text(widget, value_bool(v));
  } else if (tk_str_eq(name, PROGRESS_CIRCLE_PROP_COUNTER_CLOCK_WISE)) {
    progress_circle->is_redraw = TRUE;
    return progress_circle_set_counter_clock_wise(widget, value_bool(v));
  } else if (tk_str_eq(name, PROGRESS_CIRCLE_PROP_LINE_WIDTH)) {
    progress_circle->is_redraw = TRUE;
    return progress_circle_set_line_width(widget, value_int(v));
  } else if (tk_str_eq(name, PROGRESS_CIRCLE_PROP_START_ANGLE)) {
    progress_circle->is_redraw = TRUE;
    return progress_circle_set_start_angle(widget, value_int(v));
  } else if (tk_str_eq(name, PROGRESS_CIRCLE_PROP_LINE_CAP)) {
    progress_circle->is_redraw = TRUE;
    return progress_circle_set_line_cap(widget, value_str(v));
  } else if (tk_str_eq(name, WIDGET_PROP_W)) {
    progress_circle->is_redraw = TRUE;
    progress_circle->dirty_rect = rect_init(0, 0, value_int(v), widget->h);
  } else if (tk_str_eq(name, WIDGET_PROP_H)) {
    progress_circle->is_redraw = TRUE;
    progress_circle->dirty_rect = rect_init(0, 0, widget->w, value_int(v));
  }

  return RET_NOT_FOUND;
}

static ret_t progress_circle_init(widget_t* widget) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL, RET_BAD_PARAMS);

  progress_circle->max = 100;
  progress_circle->line_width = 8;
  progress_circle->start_angle = -90;
  progress_circle->show_text = TRUE;
  progress_circle->counter_clock_wise = FALSE;
  progress_circle->dirty_rect = rect_init(0, 0, widget->w, widget->h);
  progress_circle_set_line_cap(widget, VGCANVAS_LINE_CAP_ROUND);
  return RET_OK;
}

static const char* s_progress_circle_clone_properties[] = {WIDGET_PROP_VALUE,
                                                           WIDGET_PROP_MAX,
                                                           WIDGET_PROP_FORMAT,
                                                           PROGRESS_CIRCLE_PROP_COUNTER_CLOCK_WISE,
                                                           PROGRESS_CIRCLE_PROP_LINE_WIDTH,
                                                           PROGRESS_CIRCLE_PROP_START_ANGLE,
                                                           WIDGET_PROP_SHOW_TEXT,
                                                           NULL};
TK_DECL_VTABLE(progress_circle) = {.size = sizeof(progress_circle_t),
                                   .type = WIDGET_TYPE_PROGRESS_CIRCLE,
                                   .clone_properties = s_progress_circle_clone_properties,
                                   .get_parent_vt = TK_GET_PARENT_VTABLE(widget),
                                   .create = progress_circle_create,
                                   .init = progress_circle_init,
                                   .on_paint_self = progress_circle_on_paint_self,
                                   .on_paint_background = progress_circle_on_paint_background,
                                   .on_destroy = progress_circle_on_destroy,
                                   .get_prop = progress_circle_get_prop,
                                   .set_prop = progress_circle_set_prop};

widget_t* progress_circle_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(progress_circle), x, y, w, h);
  return_value_if_fail(progress_circle_init(widget) == RET_OK, NULL);

  return widget;
}

widget_t* progress_circle_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, progress_circle), NULL);

  return widget;
}

ret_t progress_circle_set_format(widget_t* widget, const char* format) {
  progress_circle_t* progress_circle = PROGRESS_CIRCLE(widget);
  return_value_if_fail(progress_circle != NULL && format != NULL, RET_BAD_PARAMS);

  progress_circle->format = tk_str_copy(progress_circle->format, format);

  return widget_invalidate(widget, NULL);
}
