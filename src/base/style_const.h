﻿/**
 * File:   style_const.h
 * Author: AWTK Develop Team
 * Brief:  const style(can not be changed)
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
 * 2018-10-27 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_STYLE_CONST_H
#define TK_STYLE_CONST_H

#include "base/style.h"

BEGIN_C_DECLS

/**
 * @class style_const_t
 * @parent style_t
 *
 * 只读的style，从theme\_t中获取数据。
 *
 * tools/theme_gen用于把XML的窗体样式数据转换成常量数据。
 *
 */
typedef struct _style_const_t {
  style_t style;
  char* state;
  const uint8_t* data;
} style_const_t;

/**
 * @method style_const_create
 * @annotation ["constructor"]
 *
 * 创建只读的style对象。
 *
 * @return {style_t*} 返回style对象。
 */
style_t* style_const_create(void);

END_C_DECLS
/* private */
const void* widget_get_const_style_data_for_state(widget_t* widget, const char* style_name,
                                                  const char* state);

#endif /*TK_STYLE_CONST_H*/
