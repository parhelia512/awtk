﻿/**
 * File:   draggable.c
 * Author: AWTK Develop Team
 * Brief:  make parent widget or window draggable
 *
 * Copyright (c) 2019 - 2025 Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2019-10-25 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "draggable.h"

ret_t draggable_set_top(widget_t* widget, int32_t top) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->top = top;

  return RET_OK;
}

ret_t draggable_set_bottom(widget_t* widget, int32_t bottom) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->bottom = bottom;

  return RET_OK;
}

ret_t draggable_set_left(widget_t* widget, int32_t left) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->left = left;

  return RET_OK;
}

ret_t draggable_set_right(widget_t* widget, int32_t right) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->right = right;

  return RET_OK;
}

ret_t draggable_set_vertical_only(widget_t* widget, bool_t vertical_only) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->vertical_only = vertical_only;

  return RET_OK;
}

ret_t draggable_set_horizontal_only(widget_t* widget, bool_t horizontal_only) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->horizontal_only = horizontal_only;

  return RET_OK;
}

ret_t draggable_set_allow_out_of_screen(widget_t* widget, bool_t allow_out_of_screen) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->allow_out_of_screen = allow_out_of_screen;

  return RET_OK;
}

ret_t draggable_set_drag_window(widget_t* widget, bool_t drag_window) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->drag_window = drag_window;

  return RET_OK;
}

ret_t draggable_set_drag_native_window(widget_t* widget, bool_t drag_native_window) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->drag_native_window = drag_native_window;

  return RET_OK;
}

ret_t draggable_set_drag_parent(widget_t* widget, uint32_t drag_parent) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->drag_parent = drag_parent;

  return RET_OK;
}

static ret_t draggable_get_prop(widget_t* widget, const char* name, value_t* v) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(DRAGGABLE_PROP_TOP, name)) {
    value_set_int32(v, draggable->top);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_BOTTOM, name)) {
    value_set_int32(v, draggable->bottom);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_LEFT, name)) {
    value_set_int32(v, draggable->left);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_RIGHT, name)) {
    value_set_int32(v, draggable->right);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_VERTICAL_ONLY, name)) {
    value_set_bool(v, draggable->vertical_only);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_HORIZONTAL_ONLY, name)) {
    value_set_bool(v, draggable->horizontal_only);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_DRAG_WINDOW, name)) {
    value_set_bool(v, draggable->drag_window);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_DRAG_NATIVE_WINDOW, name)) {
    value_set_bool(v, draggable->drag_native_window);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_DRAG_PARENT, name)) {
    value_set_uint32(v, draggable->drag_parent);
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_ALLOW_OUT_OF_SCREEN, name)) {
    value_set_bool(v, draggable->allow_out_of_screen);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t draggable_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(DRAGGABLE_PROP_TOP, name)) {
    draggable_set_top(widget, value_int32(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_BOTTOM, name)) {
    draggable_set_bottom(widget, value_int32(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_LEFT, name)) {
    draggable_set_left(widget, value_int32(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_RIGHT, name)) {
    draggable_set_right(widget, value_int32(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_VERTICAL_ONLY, name)) {
    draggable_set_vertical_only(widget, value_bool(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_HORIZONTAL_ONLY, name)) {
    draggable_set_horizontal_only(widget, value_bool(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_DRAG_WINDOW, name)) {
    draggable_set_drag_window(widget, value_bool(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_DRAG_NATIVE_WINDOW, name)) {
    draggable_set_drag_native_window(widget, value_bool(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_DRAG_PARENT, name)) {
    draggable_set_drag_parent(widget, value_uint32(v));
    return RET_OK;
  } else if (tk_str_eq(DRAGGABLE_PROP_ALLOW_OUT_OF_SCREEN, name)) {
    draggable_set_allow_out_of_screen(widget, value_bool(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t draggable_on_destroy(widget_t* widget) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(widget != NULL && draggable != NULL, RET_BAD_PARAMS);

  (void)draggable;

  return RET_OK;
}

static ret_t draggable_on_paint_self(widget_t* widget, canvas_t* c) {
  return RET_OK;
}

static widget_t* draggable_get_target(widget_t* widget) {
  widget_t* target = NULL;
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, NULL);

  if (draggable->drag_window) {
    target = widget_get_window(widget);
  } else {
    uint32_t i = draggable->drag_parent;
    target = widget->parent;
    while (i > 0 && target != NULL && target->parent != NULL) {
      target = target->parent;
      if (widget_is_window(target)) {
        break;
      }

      i--;
    }
  }

  return target;
}

static ret_t draggable_on_parent_pointer_down(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);
  if (widget->enable) {
    pointer_event_t* evt = (pointer_event_t*)e;
    draggable_t* draggable = DRAGGABLE(widget);
    return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);
    widget_t* target = draggable_get_target(widget);

    draggable->pressed = TRUE;
    draggable->down.x = evt->x;
    draggable->down.y = evt->y;

    if (draggable->drag_native_window) {
      native_window_info_t info;
      native_window_t* nw = widget_get_native_window(widget);
      native_window_get_info(nw, &info);
      draggable->saved_position.x = info.x;
      draggable->saved_position.y = info.y;
    } else {
      draggable->saved_position.x = target->x;
      draggable->saved_position.y = target->y;
    }

    widget_grab(widget->parent->parent, widget->parent);

    widget_dispatch_simple_event(widget, EVT_DRAG_START);
  }

  return RET_OK;
}

static ret_t draggable_move_target(widget_t* widget, xy_t x, xy_t y) {
  widget_t* target = NULL;
  draggable_t* draggable = DRAGGABLE(widget);
  native_window_t* nw = widget_get_native_window(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  target = draggable_get_target(widget);
  return_value_if_fail(target != NULL, RET_BAD_PARAMS);

  xy_t min_x = draggable->left != DRAGGABLE_UNSPECIFIED_NUM
                   ? draggable->left
                   : (draggable->allow_out_of_screen ? 1 - target->w : 0);
  xy_t min_y = draggable->top != DRAGGABLE_UNSPECIFIED_NUM
                   ? draggable->top
                   : (draggable->allow_out_of_screen ? 1 - target->h : 0);
  xy_t max_x =
      draggable->right != DRAGGABLE_UNSPECIFIED_NUM
          ? draggable->right - target->w
          : (draggable->allow_out_of_screen ? nw->rect.w - 1 : target->parent->w - target->w);
  xy_t max_y =
      draggable->bottom != DRAGGABLE_UNSPECIFIED_NUM
          ? draggable->bottom - target->h
          : (draggable->allow_out_of_screen ? nw->rect.h - 1 : target->parent->h - target->h);

  if (min_x < max_x) {
    x = tk_clampi(x, min_x, max_x);
  }

  if (min_y < max_y) {
    y = tk_clampi(y, min_y, max_y);
  }

  widget_move(target, x, y);

  return RET_OK;
}

static ret_t draggable_on_parent_pointer_move(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(widget != NULL && draggable != NULL, RET_BAD_PARAMS);

  if (widget->enable && draggable->pressed) {
    xy_t x = 0;
    xy_t y = 0;
    pointer_event_t* evt = (pointer_event_t*)e;
    xy_t dx = evt->x - draggable->down.x;
    xy_t dy = evt->y - draggable->down.y;

    x = draggable->saved_position.x + (draggable->vertical_only ? 0 : dx);
    y = draggable->saved_position.y + (draggable->horizontal_only ? 0 : dy);

    if (draggable->drag_native_window) {
      native_window_t* nw = widget_get_native_window(widget);
      native_window_move(nw, x, y, TRUE);
    } else {
      draggable_move_target(widget, x, y);
    }

    widget_dispatch_simple_event(widget, EVT_DRAG);
  }

  return RET_OK;
}

static ret_t draggable_on_parent_pointer_up(void* ctx, event_t* e) {
  widget_t* widget = WIDGET(ctx);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  if (widget->enable) {
    int32_t dx, dy;
    widget_t* target = NULL;
    draggable_t* draggable = DRAGGABLE(widget);
    return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

    draggable->pressed = FALSE;
    target = draggable_get_target(widget);
    dx = target->x - draggable->saved_position.x;
    dy = target->y - draggable->saved_position.y;

    if (tk_abs(dx) > 5 || tk_abs(dy) > 5) {
      pointer_event_t abort;
      pointer_event_init(&abort, EVT_POINTER_DOWN_ABORT, NULL, 0, 0);
      widget_dispatch(widget->parent, (event_t*)&abort);
    }

    widget_ungrab(widget->parent->parent, widget->parent);

    widget_dispatch_simple_event(widget, EVT_DRAG_END);
  }

  return RET_OK;
}

static ret_t draggable_on_attach_parent(widget_t* widget, widget_t* parent) {
  draggable_t* draggable = DRAGGABLE(widget);

  widget_on(parent, EVT_POINTER_DOWN_BEFORE_CHILDREN, draggable_on_parent_pointer_down, draggable);
  widget_on(parent, EVT_POINTER_MOVE_BEFORE_CHILDREN, draggable_on_parent_pointer_move, draggable);
  widget_on(parent, EVT_POINTER_UP_BEFORE_CHILDREN, draggable_on_parent_pointer_up, draggable);

  return RET_OK;
}

static ret_t draggable_on_detach_parent(widget_t* widget, widget_t* parent) {
  widget_off_by_ctx(parent, widget);

  return RET_OK;
}

static ret_t draggable_on_event(widget_t* widget, event_t* e) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(widget != NULL && draggable != NULL, RET_BAD_PARAMS);

  (void)draggable;

  return RET_OK;
}

static ret_t draggable_init(widget_t* widget) {
  draggable_t* draggable = DRAGGABLE(widget);
  return_value_if_fail(draggable != NULL, RET_BAD_PARAMS);

  draggable->top = DRAGGABLE_UNSPECIFIED_NUM;
  draggable->bottom = DRAGGABLE_UNSPECIFIED_NUM;
  draggable->left = DRAGGABLE_UNSPECIFIED_NUM;
  draggable->right = DRAGGABLE_UNSPECIFIED_NUM;
  draggable->vertical_only = FALSE;
  draggable->horizontal_only = FALSE;
  draggable->drag_window = FALSE;
  draggable->pressed = FALSE;
  draggable->allow_out_of_screen = FALSE;
  widget_set_sensitive(widget, FALSE);
  return RET_OK;
}

const char* s_draggable_properties[] = {
    DRAGGABLE_PROP_TOP,           DRAGGABLE_PROP_BOTTOM,
    DRAGGABLE_PROP_LEFT,          DRAGGABLE_PROP_RIGHT,
    DRAGGABLE_PROP_VERTICAL_ONLY, DRAGGABLE_PROP_HORIZONTAL_ONLY,
    DRAGGABLE_PROP_DRAG_WINDOW,   NULL};

TK_DECL_VTABLE(draggable) = {.size = sizeof(draggable_t),
                             .type = WIDGET_TYPE_DRAGGABLE,
                             .clone_properties = s_draggable_properties,
                             .persistent_properties = s_draggable_properties,
                             .get_parent_vt = TK_GET_PARENT_VTABLE(widget),
                             .create = draggable_create,
                             .init = draggable_init,
                             .on_paint_self = draggable_on_paint_self,
                             .set_prop = draggable_set_prop,
                             .get_prop = draggable_get_prop,
                             .on_event = draggable_on_event,
                             .on_attach_parent = draggable_on_attach_parent,
                             .on_detach_parent = draggable_on_detach_parent,
                             .on_destroy = draggable_on_destroy};

widget_t* draggable_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(draggable), x, y, w, h);
  return_value_if_fail(draggable_init(widget) == RET_OK, NULL);

  return widget;
}

widget_t* draggable_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, draggable), NULL);

  return widget;
}

#include "base/widget_factory.h"

ret_t draggable_register(void) {
  return widget_factory_register(widget_factory(), WIDGET_TYPE_DRAGGABLE, draggable_create);
}
