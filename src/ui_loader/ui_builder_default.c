﻿/**
 * File:   ui_builder_default.c
 * Author: AWTK Develop Team
 * Brief:  ui_builder default
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
 * 2018-02-14 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/utf8.h"
#include "base/enums.h"
#include "base/dialog.h"
#include "base/widget_factory.h"
#include "base/window_manager.h"
#include "ui_loader/ui_builder_default.h"
#include "ui_loader/ui_loader_default.h"

static ret_t ui_builder_default_on_widget_start(ui_builder_t* b, const widget_desc_t* desc) {
  const rect_t* layout = &(desc->layout);

  xy_t x = layout->x;
  xy_t y = layout->y;
  wh_t w = layout->w;
  wh_t h = layout->h;
  widget_t* widget = NULL;
  widget_t* parent = b->widget;
  const char* type = desc->type;

  widget = widget_factory_create_widget(widget_factory(), type, parent, x, y, w, h);
  if (widget == NULL) {
    log_debug("%s: not supported type %s\n", __FUNCTION__, type);
    widget = widget_factory_create_widget(widget_factory(), WIDGET_TYPE_VIEW, parent, x, y, w, h);
  }
  return_value_if_fail(widget != NULL, RET_OOM);

  b->widget = widget;
  b->widget->loading = TRUE;
  if (b->root == NULL) {
    b->root = widget;
  }

  return RET_OK;
}

static ret_t ui_builder_default_on_widget_prop(ui_builder_t* b, const char* name,
                                               const char* value) {
  value_t v;
  value_set_str(&v, value);
  widget_set_prop(b->widget, name, &v);

  return RET_OK;
}

static ret_t ui_builder_default_on_widget_prop_end(ui_builder_t* b) {
  return RET_OK;
}

static ret_t ui_builder_default_on_widget_end(ui_builder_t* b) {
  if (b->widget != NULL) {
    event_t e = event_init(EVT_WIDGET_LOAD, NULL);
    widget_dispatch(b->widget, &e);

    b->widget->loading = FALSE;
    b->widget = b->widget->parent;
  }

  return RET_OK;
}

static ret_t ui_builder_default_on_end(ui_builder_t* b) {
  ENSURE(b);
  widget_t* widget = b->root;
  if (widget != NULL) {
    widget_t* wm = window_manager();
    event_t e;

    widget_invalidate_force(widget, NULL);
    if (widget && (widget->name == NULL || widget->name[0] == 0)) {
      widget_set_name(widget, b->name);
    }

    e = event_init(EVT_WIDGET_LOAD, NULL);
    widget_dispatch(widget, &e);

    if (widget->vt->is_window) {
      e = event_init(EVT_WINDOW_LOAD, widget);
      widget_dispatch_recursive(widget, &e);
    }

    if (wm != NULL) {
      ui_load_event_t evt;
      widget_dispatch(wm, ui_load_event_init(&evt, NULL, widget, b->name));
    }

    widget->loading = FALSE;
  }

  return RET_OK;
}

static ret_t ui_builder_default_destroy(ui_builder_t* b) {
  TKMEM_FREE(b);
  return RET_OK;
}

ui_builder_t* ui_builder_default_create(const char* name) {
  ui_builder_t* builder = TKMEM_ZALLOC(ui_builder_t);
  return_value_if_fail(builder != NULL, NULL);

  builder->on_widget_start = ui_builder_default_on_widget_start;
  builder->on_widget_prop = ui_builder_default_on_widget_prop;
  builder->on_widget_prop_end = ui_builder_default_on_widget_prop_end;
  builder->on_widget_end = ui_builder_default_on_widget_end;
  builder->on_end = ui_builder_default_on_end;
  builder->destroy = ui_builder_default_destroy;
  builder->name = name;

  return builder;
}
