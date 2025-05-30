﻿/**
 * File:   row.h
 * Author: AWTK Develop Team
 * Brief:  row
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
 * 2018-08-29 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "widgets/row.h"

TK_DECL_VTABLE(row) = {.size = sizeof(row_t),
                       .type = WIDGET_TYPE_ROW,
                       .get_parent_vt = TK_GET_PARENT_VTABLE(widget),
                       .create = row_create};

widget_t* row_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  return widget_create(parent, TK_REF_VTABLE(row), x, y, w, h);
}

widget_t* row_cast(widget_t* widget) {
  return_value_if_fail(WIDGET_IS_INSTANCE_OF(widget, row), NULL);

  return widget;
}
