﻿#include "awtk.h"

static inline ret_t on_paint_rect(void* ctx, event_t* e) {
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  canvas_set_fill_color(c, color_init(0, 0, 0xff, 0));
  canvas_fill_rect(c, 10, 10, 100, 100);

  return RET_OK;
}

static inline ret_t on_paint_patch9(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;

  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, 200, 30);
  canvas_draw_image_patch9(c, &img, &r);

  r = rect_init(10, 70, 200, 60);
  canvas_draw_image_patch9(c, &img, &r);

  r = rect_init(10, 150, 20, 60);
  canvas_draw_image_patch9(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_default(void* ctx, event_t* e) {
  rect_t s;
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "earth", &img);

  s = rect_init(0, 0, img.w, img.h);
  r = rect_init(10, 220, img.w / 2, img.h / 2);
  canvas_draw_image(c, &img, &s, &r);

  s = rect_init(0, 0, img.w, img.h);
  r = rect_init(60, 220, img.w, img.h);
  canvas_draw_image(c, &img, &s, &r);

  s = rect_init(0, 0, img.w, img.h);
  r = rect_init(160, 220, img.w * 2, img.h * 2);
  canvas_draw_image(c, &img, &s, &r);

  return RET_OK;
}

static inline ret_t on_paint_line(void* ctx, event_t* e) {
  color_t fill_color = color_init(0xe0, 0xe0, 0xe0, 0xff);
  color_t stroke_color = color_init(0x80, 0x80, 0x80, 0xff);
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  canvas_set_fill_color(c, fill_color);
  canvas_set_stroke_color(c, stroke_color);

  canvas_set_stroke_color(c, stroke_color);
  canvas_draw_line(c, 10, 10, 100, 300);

  return RET_OK;
}

static inline ret_t on_paint_points(void* ctx, event_t* e) {
  point_t points[] = {{100, 100}, {100, 101}, {100, 102}, {100, 103}};
  color_t fill_color = color_init(0xe0, 0xe0, 0xe0, 0xff);
  color_t stroke_color = color_init(0x80, 0x80, 0x80, 0xff);
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  canvas_set_fill_color(c, fill_color);

  canvas_set_stroke_color(c, stroke_color);
  canvas_draw_points(c, points, ARRAY_SIZE(points));

  return RET_OK;
}

static inline ret_t on_paint_patch3_h(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, 100, 60);
  canvas_draw_image_patch3_x(c, &img, &r);

  r = rect_init(10, 80, 10, 60);
  canvas_draw_image_patch3_x(c, &img, &r);

  r = rect_init(10, 160, 300, 50);
  canvas_draw_image_patch3_x(c, &img, &r);

  r = rect_init(10, 220, 300, 20);
  canvas_draw_image_patch3_x(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_patch3_v(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, 60, 100);
  canvas_draw_image_patch3_x(c, &img, &r);

  r = rect_init(80, 10, 60, 10);
  canvas_draw_image_patch3_x(c, &img, &r);

  r = rect_init(160, 10, 50, 300);
  canvas_draw_image_patch3_x(c, &img, &r);

  r = rect_init(220, 10, 20, 300);
  canvas_draw_image_patch3_x(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_repeat_x(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, 10, img.h);
  canvas_draw_image_repeat_x(c, &img, &r);

  r = rect_init(10, 60, img.w, img.h);
  canvas_draw_image_repeat_x(c, &img, &r);

  r = rect_init(10, 160, img.w * 2, img.h * 2);
  canvas_draw_image_repeat_x(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_repeat_y(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, img.w, 10);
  canvas_draw_image_repeat_y(c, &img, &r);

  r = rect_init(90, 10, img.w, img.h);
  canvas_draw_image_repeat_y(c, &img, &r);

  r = rect_init(160, 10, img.w * 2, img.h * 2);
  canvas_draw_image_repeat_y(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_repeat(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, img.w / 2, img.h / 2);
  canvas_draw_image_repeat(c, &img, &r);

  r = rect_init(50, 50, img.w, img.h);
  canvas_draw_image_repeat(c, &img, &r);

  r = rect_init(100, 100, img.w * 2 + 10, img.h * 2 + 10);
  canvas_draw_image_repeat(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_scale_x(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, img.w, img.h / 2);
  canvas_draw_image_scale_w(c, &img, &r);

  r = rect_init(50, 50, img.w, img.h);
  canvas_draw_image_scale_w(c, &img, &r);

  r = rect_init(100, 100, img.w * 2, img.h);
  canvas_draw_image_scale_w(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_scale_y(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, img.w / 2, img.h);
  canvas_draw_image_scale_h(c, &img, &r);

  r = rect_init(50, 50, img.w, img.h);
  canvas_draw_image_scale_h(c, &img, &r);

  r = rect_init(100, 100, img.w, img.h * 2);
  canvas_draw_image_scale_h(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_scale(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, img.w / 2, img.h);
  canvas_draw_image_scale(c, &img, &r);

  r = rect_init(50, 50, img.w, img.h);
  canvas_draw_image_scale(c, &img, &r);

  r = rect_init(100, 100, img.w, img.h * 2);
  canvas_draw_image_scale(c, &img, &r);

  return RET_OK;
}

static inline ret_t on_paint_center(void* ctx, event_t* e) {
  rect_t r;
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  image_manager_get_bitmap(image_manager(), "btn_blue_n", &img);

  r = rect_init(10, 10, img.w / 2, img.h);
  canvas_draw_image_center(c, &img, &r);

  r = rect_init(50, 50, img.w, img.h);
  canvas_draw_image_center(c, &img, &r);

  r = rect_init(100, 100, img.w, img.h * 2);
  canvas_draw_image_center(c, &img, &r);

  return RET_OK;
}

static void draw_basic_shapes(vgcanvas_t* vg, bool_t stroke) {
  vgcanvas_save(vg);

  vgcanvas_translate(vg, 5, 5);
  vgcanvas_rounded_rect(vg, 0, 0, 60, 40, 5);
  if (stroke)
    vgcanvas_stroke(vg);
  else
    vgcanvas_fill(vg);

  vgcanvas_translate(vg, 65, 0);
  vgcanvas_rounded_rect(vg, 0, 0, 60, 40, 1);
  if (stroke)
    vgcanvas_stroke(vg);
  else
    vgcanvas_fill(vg);

  vgcanvas_translate(vg, 65, 0);
  vgcanvas_ellipse(vg, 30, 20, 30, 20);
  if (stroke)
    vgcanvas_stroke(vg);
  else
    vgcanvas_fill(vg);

  vgcanvas_begin_path(vg);
  vgcanvas_translate(vg, 65, 0);
  vgcanvas_arc(vg, 20, 20, 20, 0, 2 * 3.15, FALSE);
  if (stroke)
    vgcanvas_stroke(vg);
  else
    vgcanvas_fill(vg);

  vgcanvas_begin_path(vg);
  vgcanvas_translate(vg, 50, 0);
  vgcanvas_move_to(vg, 0, 0);
  vgcanvas_line_to(vg, 40, 0);
  vgcanvas_line_to(vg, 40, 40);
  vgcanvas_close_path(vg);
  if (stroke)
    vgcanvas_stroke(vg);
  else
    vgcanvas_fill(vg);

  vgcanvas_begin_path(vg);
  vgcanvas_restore(vg);
}

static ret_t build_dashed_line_path(vgcanvas_t* vg, point_t p1, point_t p2, uint32_t on,
                                    uint32_t off) {
  float x = p1.x;
  float y = p1.y;
  float ddx_on = 0;
  float ddy_on = 0;
  float ddx_off = 0;
  float ddy_off = 0;
  bool_t done = FALSE;
  int32_t dx = p2.x - p1.x;
  int32_t dy = p2.y - p1.y;
  return_value_if_fail(vg != NULL && on != 0 && off != 0, RET_BAD_PARAMS);

  if (dx == 0 && dy == 0) {
    return RET_OK;
  }

  if (dx == 0) {
    /*垂直*/
    ddx_on = 0;
    ddx_off = 0;
    if (dy < 0) {
      ddy_on = -(int)on;
      ddy_off = -(int)off;
    } else {
      ddy_on = on;
      ddy_off = off;
    }
  } else if (dy == 0) {
    /*水平*/
    ddy_on = 0;
    ddy_off = 0;
    if (dx < 0) {
      ddx_on = -(int)on;
      ddx_off = -(int)off;
    } else {
      ddx_on = on;
      ddx_off = off;
    }
  } else {
    float len = sqrt(dx * dx + dy * dy);
    float scalex = dx / len;
    float scaley = dy / len;
    ddy_on = scaley * on;
    ddy_off = scaley * off;
    ddx_on = scalex * on;
    ddx_off = scalex * off;
  }

  while (!done) {
    if ((dx > 0 && x >= p2.x) || (dx <= 0 && x < p2.x)) {
      break;
    }
    if ((dy > 0 && y >= p2.y) || (dy <= 0 && y < p2.y)) {
      break;
    }
    vgcanvas_move_to(vg, x, y);
    x += ddx_on;
    y += ddy_on;
    if (dx > 0) {
      x = tk_min(x, p2.x);
    } else {
      x = tk_max(x, p2.x);
    }
    if (dy > 0) {
      y = tk_min(y, p2.y);
    } else {
      y = tk_max(y, p2.y);
    }
    vgcanvas_line_to(vg, x, y);
    x += ddx_off;
    y += ddy_off;
  }

  return RET_OK;
}

static void stroke_lines(vgcanvas_t* vg) {
  vgcanvas_save(vg);

  vgcanvas_set_stroke_color(vg, color_init(0, 0, 0xff, 0xff));
  vgcanvas_move_to(vg, 0, 0);
  vgcanvas_line_to(vg, 40, 40);

  vgcanvas_translate(vg, 40, 0);
  vgcanvas_move_to(vg, 0, 0);
  vgcanvas_quad_to(vg, 40, 0, 40, 40);

  vgcanvas_translate(vg, 40, 0);
  vgcanvas_move_to(vg, 0, 0);
  vgcanvas_bezier_to(vg, 20, 0, 20, 40, 40, 40);

  vgcanvas_stroke(vg);

  vgcanvas_translate(vg, 40, 0);
  vgcanvas_begin_path(vg);
  vgcanvas_arc(vg, 20, 20, 20, 0, 3.14, TRUE);
  vgcanvas_stroke(vg);

  vgcanvas_translate(vg, 40, 0);
  vgcanvas_set_line_width(vg, 2);
  vgcanvas_set_line_cap(vg, VGCANVAS_LINE_CAP_ROUND);
  vgcanvas_begin_path(vg);
  vgcanvas_arc(vg, 20, 20, 20, 0, 3.14 / 2, FALSE);
  vgcanvas_stroke(vg);

  vgcanvas_stroke(vg);

  /*************************************************/
  vgcanvas_translate(vg, 50, 0);
  vgcanvas_save(vg);
  vgcanvas_set_line_width(vg, 2);
  vgcanvas_set_line_cap(vg, VGCANVAS_LINE_CAP_BUTT);

  point_t points[] = {
      {0, 20}, {8, 20}, {8, 20}, {8 * 2, 20}, {8 * 3, 20}, {8 * 4, 20}, {8 * 4, 20}, {8 * 5, 20},
  };

  vgcanvas_begin_path(vg);
  for (size_t start = 1, i = start; i < ARRAY_SIZE(points); i++) {
    if (i == start) {
      vgcanvas_move_to(vg, points[i].x, points[i].y);
    } else {
      vgcanvas_line_to(vg, points[i].x, points[i].y);
    }
  }
  vgcanvas_stroke(vg);

  vgcanvas_begin_path(vg);
  vgcanvas_move_to(vg, points[0].x, points[0].y);
  vgcanvas_line_to(vg, points[1].x, points[1].y);
  vgcanvas_stroke(vg);
  vgcanvas_restore(vg);
  /*************************************************/
  const uint32_t dashed_on = 2;
  const uint32_t dashed_off = 2;
  vgcanvas_translate(vg, 50, 0);
  vgcanvas_save(vg);
  vgcanvas_set_stroke_color(vg, color_parse("violet"));

  vgcanvas_begin_path(vg);

  vgcanvas_move_to(vg, 0, 0);

  build_dashed_line_path(vg, point_init(0, 0), point_init(50, 0), dashed_on, dashed_off);

  build_dashed_line_path(vg, point_init(0, 0), point_init(0, 50), dashed_on, dashed_off);

  build_dashed_line_path(vg, point_init(0, 0), point_init(50, 50), dashed_on, dashed_off);

  build_dashed_line_path(vg, point_init(0, 0), point_init(50, 10), dashed_on, dashed_off);

  build_dashed_line_path(vg, point_init(0, 0), point_init(25, 50), dashed_on, dashed_off);

  vgcanvas_stroke(vg);
  vgcanvas_restore(vg);
  /*************************************************/

  vgcanvas_restore(vg);
}

static void draw_image(vgcanvas_t* vg, const char* name) {
  bitmap_t img;

  vgcanvas_save(vg);

  image_manager_get_bitmap(image_manager(), name, &img);

  vgcanvas_translate(vg, (vg->w - img.w) / 2, 0);

  vgcanvas_translate(vg, img.w >> 1, img.h >> 1);
  vgcanvas_rotate(vg, 3.14 / 4);
  vgcanvas_translate(vg, -img.w >> 1, -img.h >> 1);

  vgcanvas_scale(vg, 1.5, 1.5);
  vgcanvas_draw_image(vg, &img, 0, 0, img.w, img.h, 0, 0, img.w, img.h);
  vgcanvas_restore(vg);

  return;
}

static void draw_matrix(vgcanvas_t* vg) {
  float_t w = 50;
  float_t h = 50;

  vgcanvas_save(vg);
  vgcanvas_translate(vg, w / 2, h / 2);
  vgcanvas_rotate(vg, 3.14 / 4);
  vgcanvas_translate(vg, -w / 2, -h / 2);

  vgcanvas_rect(vg, 0, 0, w, h);
  vgcanvas_fill(vg);
  vgcanvas_restore(vg);
}

static void draw_text(vgcanvas_t* vg) {
  float_t h = 20;
  const char* text = "Hello AWTK";
  float_t w = 100;

  vgcanvas_set_font_size(vg, 20);
  vgcanvas_set_font(vg, NULL);
  w = vgcanvas_measure_text(vg, text);

  vgcanvas_save(vg);
  vgcanvas_translate(vg, w / 2, h / 2);
  vgcanvas_rotate(vg, 3.14 / 4);
  vgcanvas_translate(vg, -w / 2, -h / 2);

  vgcanvas_fill_text(vg, text, 10, 10, 100);
  log_debug("text=%s w=%f\n", text, w);
  vgcanvas_restore(vg);

  vgcanvas_save(vg);

  vgcanvas_translate(vg, w * 3, h);
  vgcanvas_fill_text(vg, text, 10, 10, 100);
  vgcanvas_save(vg);
  vgcanvas_scale(vg, -1, 1);
  vgcanvas_fill_text(vg, text, 10, 10, 100);
  vgcanvas_restore(vg);

  vgcanvas_save(vg);
  vgcanvas_scale(vg, 1, -1);
  vgcanvas_fill_text(vg, text, 10, 10, 100);
  vgcanvas_restore(vg);

  vgcanvas_save(vg);
  vgcanvas_scale(vg, -1, -1);
  vgcanvas_fill_text(vg, text, 10, 10, 100);
  vgcanvas_restore(vg);

  vgcanvas_restore(vg);
}

static void draw_fill_mode_test(vgcanvas_t* vg, vgcanvas_fill_mode_t fill_mode) {
  vgcanvas_save(vg);

  vgcanvas_begin_path(vg);

  vgcanvas_move_to(vg, 0, 0);
  vgcanvas_line_to(vg, 40, 0);
  vgcanvas_line_to(vg, 40, 40);
  vgcanvas_line_to(vg, 0, 40);
  vgcanvas_close_path(vg);

  vgcanvas_move_to(vg, 10, 20);
  vgcanvas_line_to(vg, 10, 40);
  vgcanvas_line_to(vg, 30, 40);
  vgcanvas_line_to(vg, 30, 20);
  vgcanvas_close_path(vg);

  vgcanvas_move_to(vg, 15, 15);
  vgcanvas_line_to(vg, 15, 25);
  vgcanvas_line_to(vg, 25, 25);
  vgcanvas_line_to(vg, 25, 15);
  vgcanvas_close_path(vg);

  // 因为星星路径多个区域出现交叠，如果使用 VGCANVAS_FILL_MODE_ALL_FILL 的话，就不会是一个星星的图案了。
  if (fill_mode != VGCANVAS_FILL_MODE_ALL_FILL) {
    vgcanvas_translate(vg, 0, 50);
    vgcanvas_scale(vg, 0.2f, 0.2f);
    vgcanvas_move_to(vg, 100, 0);
    vgcanvas_line_to(vg, 159, 181);
    vgcanvas_line_to(vg, 5, 69);
    vgcanvas_line_to(vg, 195, 69);
    vgcanvas_line_to(vg, 41, 181);
    vgcanvas_line_to(vg, 100, 0);
    vgcanvas_close_path(vg);
  }

  vgcanvas_set_fill_mode(vg, fill_mode);
  vgcanvas_fill(vg);

  vgcanvas_restore(vg);
}

static void draw_fill_mode(vgcanvas_t* vg) {
  vgcanvas_save(vg);
  vgcanvas_translate(vg, 10, 0);
  draw_fill_mode_test(vg, VGCANVAS_FILL_MODE_ALL_FILL);

  vgcanvas_translate(vg, 50, 0);
  draw_fill_mode_test(vg, VGCANVAS_FILL_MODE_NON_ZERO);

  vgcanvas_translate(vg, 50, 0);
  draw_fill_mode_test(vg, VGCANVAS_FILL_MODE_EVEN_ODD);
  vgcanvas_restore(vg);
}

static inline ret_t on_paint_text(void* ctx, event_t* e) {
  int32_t x = 0;
  int32_t y = 0;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  color_t color = color_init(0xff, 0, 0, 0xff);
  vgcanvas_t* vg = canvas_get_vgcanvas(c);

  color = color_init(0xff, 0, 0, 0xff);
  canvas_set_stroke_color(c, color);
  canvas_stroke_rect(c, x, y, 100, 30);

  color = color_init(0xff, 0, 0, 0xff);
  canvas_set_text_color(c, color);
  canvas_set_font(c, NULL, 30);

  vgcanvas_set_fill_color(vg, color);
  vgcanvas_fill_text(vg, "ABC", 0, 0, 100);
  /*
    vgcanvas_translate(vg, x, y);
    canvas_draw_text(c, L"abc", 3, 0, 0);
  */
  return RET_OK;
}

static inline ret_t on_paint_vg(void* ctx, event_t* e) {
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  vgcanvas_t* vg = canvas_get_vgcanvas(c);

  vgcanvas_save(vg);
  vgcanvas_set_line_width(vg, 1);
  vgcanvas_set_stroke_color(vg, color_init(0, 0xff, 0, 0xff));
  vgcanvas_set_fill_color(vg, color_init(0xff, 0, 0, 0xff));

  draw_basic_shapes(vg, FALSE);
  vgcanvas_translate(vg, 0, 50);
  draw_basic_shapes(vg, TRUE);
  vgcanvas_translate(vg, 0, 50);
  stroke_lines(vg);
  vgcanvas_translate(vg, 0, 50);
  draw_image(vg, "bricks");
  draw_image(vg, "message");

  vgcanvas_translate(vg, 50, 100);
  draw_matrix(vg);
  vgcanvas_translate(vg, -50, 50);

  draw_text(vg);

  vgcanvas_translate(vg, 0, 60);
  draw_fill_mode(vg);

  vgcanvas_restore(vg);

  return RET_OK;
}

static inline ret_t on_paint_global_alpha(void* ctx, event_t* e) {
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  color_t fill = color_init(0x00, 0xff, 0x00, 0xff);
  color_t stroke = color_init(0x00, 0x00, 0xff, 0xff);

  vgcanvas_save(vg);

  image_manager_get_bitmap(image_manager(), "bricks", &img);
  vgcanvas_set_global_alpha(vg, 1);
  vgcanvas_draw_image(vg, &img, 0, 0, img.w, img.h, 0, 0, img.w, img.h);

  vgcanvas_set_global_alpha(vg, 0.5);

  vgcanvas_set_fill_color(vg, fill);
  vgcanvas_rect(vg, 0, 0, 20, 20);
  vgcanvas_fill(vg);

  vgcanvas_set_stroke_color(vg, stroke);
  vgcanvas_set_line_width(vg, 5);
  vgcanvas_rect(vg, 20, 20, 20, 20);
  vgcanvas_stroke(vg);

  vgcanvas_translate(vg, 15, 15);
  vgcanvas_draw_image(vg, &img, 0, 0, img.w, img.h, 0, 0, img.w, img.h);

  vgcanvas_restore(vg);

  return RET_OK;
}

static inline ret_t on_paint_vg_simple(void* ctx, event_t* e) {
  bitmap_t img;
  paint_event_t* evt = (paint_event_t*)e;
  canvas_t* c = evt->c;
  vgcanvas_t* vg = canvas_get_vgcanvas(c);
  image_manager_get_bitmap(image_manager(), "rgba", &img);

  canvas_set_fill_color(c, color_init(0x11, 0x22, 0x33, 0xff));
  canvas_fill_rect(c, 0, 0, 10, 10);

  rect_t s = rect_init(0, 0, img.w, img.h);
  rect_t r = rect_init(10, 10, img.w, img.h);
  canvas_draw_image(c, &img, &s, &r);

  vgcanvas_save(vg);
  vgcanvas_translate(vg, 0, 100);

  vgcanvas_set_line_width(vg, 1);
  vgcanvas_set_fill_color(vg, color_init(0xff, 0, 0, 0xff));
  vgcanvas_rect(vg, 5, 5, 100, 100);
  vgcanvas_fill(vg);

  vgcanvas_set_fill_color(vg, color_init(0, 0xff, 0, 0xff));
  vgcanvas_rect(vg, 110, 5, 100, 100);
  vgcanvas_fill(vg);

  vgcanvas_set_fill_color(vg, color_init(0, 0, 0xff, 0xff));
  vgcanvas_rect(vg, 215, 5, 100, 100);
  vgcanvas_fill(vg);

  vgcanvas_translate(vg, 0, 105);
  vgcanvas_set_stroke_color(vg, color_init(0xff, 0, 0, 0xff));
  vgcanvas_rect(vg, 5, 5, 100, 100);
  vgcanvas_stroke(vg);

  vgcanvas_set_stroke_color(vg, color_init(0, 0xff, 0, 0xff));
  vgcanvas_rect(vg, 110, 5, 100, 100);
  vgcanvas_stroke(vg);

  vgcanvas_set_stroke_color(vg, color_init(0, 0, 0xff, 0xff));
  vgcanvas_rect(vg, 215, 5, 100, 100);
  vgcanvas_stroke(vg);

  vgcanvas_translate(vg, 0, 128);
  image_manager_get_bitmap(image_manager(), "rgb", &img);
  vgcanvas_draw_image(vg, &img, 0, 0, img.w, img.h, 0, 0, img.w, img.h);

  image_manager_get_bitmap(image_manager(), "rgba", &img);
  vgcanvas_draw_image(vg, &img, 0, 0, img.w, img.h, 100, 0, img.w, img.h);

  image_manager_get_bitmap(image_manager(), "switch", &img);
  vgcanvas_draw_image(vg, &img, 0, 0, img.w, img.h, 200, 0, img.w, img.h);

  vgcanvas_translate(vg, 0, 60);
  vgcanvas_rounded_rect(vg, 0, 0, img.w, img.h, 5);
  vgcanvas_paint(vg, FALSE, &img);

  vg = canvas_get_vgcanvas(c);
  vgcanvas_translate(vg, 160, 0);
  vgcanvas_rounded_rect(vg, 0, 0, img.w, img.h, 5);
  vgcanvas_paint(vg, FALSE, &img);

  vgcanvas_restore(vg);

  return RET_OK;
}

static inline ret_t on_timer(const timer_info_t* timer) {
  widget_t* widget = WIDGET(timer->ctx);

  widget_invalidate(widget, NULL);

  return RET_REPEAT;
}
