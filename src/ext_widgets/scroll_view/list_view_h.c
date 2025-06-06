﻿/**
 * File:   list_view_h.h
 * Author: AWTK Develop Team
 * Brief:  list_view_h
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
 * 2018-07-04 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "base/layout.h"
#include "scroll_view/list_view_h.h"
#include "scroll_view/scroll_view.h"

static ret_t list_view_h_on_add_child(widget_t* widget, widget_t* child);
static ret_t list_view_h_on_remove_child(widget_t* widget, widget_t* child);

static ret_t list_view_h_on_paint_self(widget_t* widget, canvas_t* c) {
  return widget_paint_helper(widget, c, NULL, NULL);
}

static ret_t list_view_h_get_prop(widget_t* widget, const char* name, value_t* v) {
  list_view_h_t* list_view_h = LIST_VIEW_H(widget);
  ENSURE(list_view_h);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_ITEM_WIDTH)) {
    value_set_int(v, list_view_h->item_width);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_SPACING)) {
    value_set_int(v, list_view_h->spacing);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t list_view_h_set_prop(widget_t* widget, const char* name, const value_t* v) {
  list_view_h_t* list_view_h = LIST_VIEW_H(widget);
  ENSURE(list_view_h);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_ITEM_WIDTH)) {
    list_view_h_set_item_width(widget, value_int(v));
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_SPACING)) {
    list_view_h_set_spacing(widget, value_int(v));
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t list_view_h_on_event(widget_t* widget, event_t* e) {
  ret_t ret = RET_OK;
  list_view_h_t* list_view = LIST_VIEW_H(widget);
  return_value_if_fail(list_view != NULL, RET_BAD_PARAMS);

  switch (e->type) {
    case EVT_KEY_DOWN: {
      key_event_t* evt = (key_event_t*)e;
      if (evt->key == TK_KEY_PAGEDOWN) {
        scroll_view_scroll_delta_to(list_view->scroll_view, widget->w, 0, TK_ANIMATING_TIME);
        ret = RET_STOP;
      } else if (evt->key == TK_KEY_PAGEUP) {
        scroll_view_scroll_delta_to(list_view->scroll_view, -widget->w, 0, TK_ANIMATING_TIME);
        ret = RET_STOP;
      }
      break;
    }
    default:
      break;
  }

  return ret;
}

TK_DECL_VTABLE(list_view_h) = {.type = WIDGET_TYPE_LIST_VIEW_H,
                               .size = sizeof(list_view_h_t),
                               .get_parent_vt = TK_GET_PARENT_VTABLE(widget),
                               .create = list_view_h_create,
                               .set_prop = list_view_h_set_prop,
                               .get_prop = list_view_h_get_prop,
                               .on_event = list_view_h_on_event,
                               .on_add_child = list_view_h_on_add_child,
                               .on_remove_child = list_view_h_on_remove_child,
                               .on_paint_self = list_view_h_on_paint_self};

static ret_t list_view_h_on_scroll_view_layout_children(widget_t* widget) {
  list_view_h_t* list_view_h = LIST_VIEW_H(widget->parent);
  return_value_if_fail(list_view_h != NULL, RET_BAD_PARAMS);

  int32_t i = 0;
  int32_t x = 0;
  int32_t y = 0;
  int32_t w = list_view_h->item_width;
  int32_t h = widget->h;
  int32_t spacing = 0;
  int32_t x_margin = 0;
  int32_t max_w = 0;
  int32_t virtual_w = widget->w;

  if (widget->children_layout != NULL) {
    children_layouter_layout(widget->children_layout, widget);

    if (tk_str_start_with(children_layouter_to_string(widget->children_layout), "list_view")) {
      scroll_view_set_xslidable(list_view_h->scroll_view, TRUE);
      scroll_view_set_yslidable(list_view_h->scroll_view, FALSE);
      return RET_OK;
    }

    spacing = children_layouter_get_param_int(widget->children_layout, "spacing", 0);
    x_margin = children_layouter_get_param_int(widget->children_layout, "x_margin", 0);
  } else {
    spacing = list_view_h->spacing;
  }

  if (widget->children != NULL) {
    for (i = 0; i < widget->children->size; i++) {
      widget_t* iter = (widget_t*)darray_get(widget->children, i);

      if (widget->children_layout == NULL) {
        widget_move_resize_ex(iter, x, y, w, h, FALSE);
        widget_layout(iter);
      }
      x = iter->x + iter->w + spacing;
      max_w = tk_max(max_w, x);
    }
  }

  max_w += x_margin - spacing;
  if (max_w > virtual_w) {
    virtual_w = max_w;
  }

  scroll_view_set_virtual_w(list_view_h->scroll_view, virtual_w);
  scroll_view_set_xslidable(list_view_h->scroll_view, TRUE);
  scroll_view_set_yslidable(list_view_h->scroll_view, FALSE);

  return RET_OK;
}

static ret_t list_view_h_on_add_child(widget_t* widget, widget_t* child) {
  list_view_h_t* list_view_h = LIST_VIEW_H(widget);
  ENSURE(list_view_h);
  const char* type = widget_get_type(child);

  if (tk_str_eq(type, WIDGET_TYPE_SCROLL_VIEW)) {
    scroll_view_t* scroll_view = SCROLL_VIEW(child);

    list_view_h->scroll_view = child;
    scroll_view->on_layout_children = list_view_h_on_scroll_view_layout_children;
  }

  return RET_CONTINUE;
}

static ret_t list_view_h_on_remove_child(widget_t* widget, widget_t* child) {
  list_view_h_t* list_view_h = LIST_VIEW_H(widget);
  ENSURE(list_view_h);

  if (list_view_h->scroll_view == child) {
    scroll_view_t* scroll_view = SCROLL_VIEW(child);

    list_view_h->scroll_view = NULL;
    scroll_view->on_layout_children = NULL;

    WIDGET_FOR_EACH_CHILD_BEGIN_R(widget, iter, i)
    if (iter && iter != child && tk_str_eq(iter->vt->type, WIDGET_TYPE_SCROLL_VIEW)) {
      list_view_h->scroll_view = iter;
      scroll_view->on_layout_children = list_view_h_on_scroll_view_layout_children;
      break;
    }
    WIDGET_FOR_EACH_CHILD_END();
  }

  return RET_CONTINUE;
}

widget_t* list_view_h_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  return widget_create(parent, TK_REF_VTABLE(list_view_h), x, y, w, h);
}

ret_t list_view_h_set_item_width(widget_t* widget, int32_t item_width) {
  list_view_h_t* list_view_h = LIST_VIEW_H(widget);
  ENSURE(list_view_h);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  list_view_h->item_width = item_width;
  widget_layout_children(widget);

  return RET_OK;
}

ret_t list_view_h_set_spacing(widget_t* widget, int32_t spacing) {
  list_view_h_t* list_view_h = LIST_VIEW_H(widget);
  ENSURE(list_view_h);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  list_view_h->spacing = spacing;
  widget_layout_children(widget);

  return RET_OK;
}

widget_t* list_view_h_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, list_view_h), NULL);

  return widget;
}
