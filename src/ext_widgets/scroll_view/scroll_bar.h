﻿/**
 * File:   scroll_bar.h
 * Author: AWTK Develop Team
 * Brief:  scroll_bar
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
 * 2018-07-01 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_SCROLL_BAR_H
#define TK_SCROLL_BAR_H

#include "base/widget.h"
#include "base/widget_animator.h"

BEGIN_C_DECLS

/**
 * @class scroll_bar_t
 * @parent widget_t
 * @annotation ["scriptable","design","widget"]
 * 滚动条控件。
 *
 * scroll\_bar\_t是[widget\_t](widget_t.md)的子类控件，widget\_t的函数均适用于scroll\_bar\_t控件。
 *
 * 在xml中使用"scroll\_bar"或"scroll\_bar\_d"或"scroll\_bar\_m"标签创建滚动条控件。如：
 *
 * ```xml
 * <list_view x="0"  y="30" w="100%" h="-80" item_height="60">
 * <scroll_view name="view" x="0"  y="0" w="100%" h="100%">
 * ...
 * </scroll_view>
 * <scroll_bar_m name="bar" x="right" y="0" w="6" h="100%" value="0"/>
 * </list_view>
 * ```
 *
 * > 更多用法请参考：[list\_view\_m.xml](
 *https://github.com/zlgopen/awtk/blob/master/design/default/ui/list_view_m.xml)
 *
 * 在c代码中使用函数scroll\_bar\_create创建列表项控件。如：
 *
 * ```c
 *  widget_t* scroll_bar = scroll_bar_create(list_view, 0, 0, 0, 0);
 * ```
 *
 * ```xml
 * <style name="default">
 *   <normal bg_color="#c0c0c0" fg_color="#808080"/>
 *   <over bg_color="#c0c0c0" fg_color="#808080"/>
 *   <pressed bg_color="#c0c0c0" fg_color="#808080"/>
 * </style>
 * ```
 *
 * > 更多用法请参考：[theme default](
 *https://github.com/zlgopen/awtk/blob/master/design/default/styles/default.xml#L350)
 *
 */
typedef struct _scroll_bar_t {
  widget_t widget;
  /**
   * @property {int32_t} virtual_size
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 虚拟宽度或高度。
   */
  int32_t virtual_size;
  /**
   * @property {int32_t} value
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 当前的值。
   */
  int32_t value;
  /**
   * @property {int32_t} row
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 行的高度。
   */
  int32_t row;
  /**
   * @property {uint32_t} animator_time
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 翻页滚动动画时间(毫秒)。
   */
  uint32_t animator_time;
  /**
   * @property {uint32_t} scroll_delta
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 每次鼠标滚动值。（缺省值为0，0 则使用鼠标滚动默认值）
   */
  uint32_t scroll_delta;
  /**
   * @property {uint32_t} scroll_rows
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 每次鼠标滚动行数。（与 scroll_delta 互斥，缺省值为0，0 则使用 scroll_delta）
   */
  uint8_t scroll_rows;
  /**
   * @property {bool_t} animatable
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 滚动时是否启用动画。
   */
  bool_t animatable;
  /**
   * @property {bool_t} auto_hide
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 是否自动隐藏(仅对mobile风格的滚动条有效)。
   */
  bool_t auto_hide;
  /**
   * @property {bool_t} wheel_scroll
   * @annotation ["set_prop","get_prop","readable","persitent","design","scriptable"]
   * 设置鼠标滚轮是否滚动(仅对desktop风格的滚动条有效)（垂直滚动条缺省值为TRUE，水平滚动条缺省值为FALSE）。
   */
  bool_t wheel_scroll;

  /*private*/
  bool_t user_wheel_scroll;
  widget_t* dragger;
  widget_animator_t* wa_value;
  widget_animator_t* wa_opacity;

  uint32_t wheel_before_id;
} scroll_bar_t;

/**
 * @event {value_change_event_t} EVT_VALUE_WILL_CHANGE
 * 值(滚动值)即将改变事件。
 */

/**
 * @event {value_change_event_t} EVT_VALUE_CHANGED
 * 值(滚动值)改变事件。
 */

/**
 * @method scroll_bar_create
 * 创建scroll_bar对象
 *
 * > 根据宏WITH_DESKTOP_STYLE决定创建desktop风格还是mobile风格的滚动条
 *
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* scroll_bar_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method scroll_bar_cast
 * 转换为scroll_bar对象(供脚本语言使用)。
 * @annotation ["cast", "scriptable"]
 * @param {widget_t*} widget scroll_bar对象。
 *
 * @return {widget_t*} scroll_bar对象。
 */
widget_t* scroll_bar_cast(widget_t* widget);

/**
 * @method scroll_bar_create_mobile
 * 创建mobile风格的scroll_bar对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* scroll_bar_create_mobile(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method scroll_bar_create_desktop
 * 创建desktop风格的scroll_bar对象
 * @annotation ["constructor", "scriptable"]
 * @param {widget_t*} parent 父控件
 * @param {xy_t} x x坐标
 * @param {xy_t} y y坐标
 * @param {wh_t} w 宽度
 * @param {wh_t} h 高度
 *
 * @return {widget_t*} 对象。
 */
widget_t* scroll_bar_create_desktop(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h);

/**
 * @method scroll_bar_set_params
 * 设置参数。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} virtual_size 虚拟高度。
 * @param {int32_t} row 每一行的高度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_params(widget_t* widget, int32_t virtual_size, int32_t row);

/**
 * @method scroll_bar_scroll_to
 * 滚动到指定的值。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} value 值。
 * @param {int32_t} duration 动画持续时间(毫秒)。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_scroll_to(widget_t* widget, int32_t value, int32_t duration);

/**
 * @method scroll_bar_set_value
 * 设置值，并触发EVT_VALUE_CHANGED事件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_value(widget_t* widget, int32_t value);

/**
 * @method scroll_bar_add_delta
 * 在当前的值上增加一个值，并触发EVT_VALUE_CHANGED事件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} delta 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_add_delta(widget_t* widget, int32_t delta);

/**
 * @method scroll_bar_scroll_delta
 * 在当前的值上增加一个值，并滚动到新的值，并触发EVT_VALUE_CHANGED事件。
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} delta 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_scroll_delta(widget_t* widget, int32_t delta);

/**
 * @method scroll_bar_set_value_only
 * 设置值，但不触发EVT_VALUE_CHANGED事件。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} value 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_value_only(widget_t* widget, int32_t value);

/**
 * @method scroll_bar_set_auto_hide
 * 设置auto_hide属性。
 *
 *>仅对mobile风格的滚动条有效
 *
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {bool_t} auto_hide 值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_auto_hide(widget_t* widget, bool_t auto_hide);

/**
 * @method scroll_bar_is_mobile
 * 判断是否是mobile风格的滚动条。
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 *
 * @return {bool_t} 返回TRUE表示是mobile风格的，否则表示不是mobile风格的。
 */
bool_t scroll_bar_is_mobile(widget_t* widget);

/**
 * @method scroll_bar_set_animator_time
 * 设置翻页滚动动画时间(毫秒)。
 *
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {uint32_t} animator_time 时间(毫秒)。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_animator_time(widget_t* widget, uint32_t animator_time);

/**
 * @method scroll_bar_hide_by_opacity_animation
 * 通过动画隐藏滚动条。
 *
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} duration 动画持续时间(毫秒)。
 * @param {int32_t} delay 动画执行时间(毫秒)。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_hide_by_opacity_animation(widget_t* widget, int32_t duration, int32_t delay);

/**
 * @method scroll_bar_show_by_opacity_animation
 * 通过动画显示滚动条。
 *
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {int32_t} duration 动画持续时间(毫秒)。
 * @param {int32_t} delay 动画执行时间(毫秒)。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_show_by_opacity_animation(widget_t* widget, int32_t duration, int32_t delay);

/**
 * @method scroll_bar_set_wheel_scroll
 * 设置鼠标滚轮是否滚动(仅对desktop风格的滚动条有效)。
 * 
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {bool_t} scroll 是否设置该功能。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_wheel_scroll(widget_t* widget, bool_t scroll);

/**
 * @method scroll_bar_set_scroll_delta
 * 设置鼠标滚轮幅度(仅对desktop风格的滚动条有效)。
 * 
 * @annotation ["scriptable"]
 * @param {widget_t*} widget scroll_bar控件。
 * @param {uint32_t} scroll_delta 滚动幅度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t scroll_bar_set_scroll_delta(widget_t* widget, uint32_t scroll_delta);

/* private */

#define SCROLL_BAR_PROP_IS_MOBILE "is_mobile"
#define SCROLL_BAR_PROP_IS_HORIZON "is_horizon"
#define SCROLL_BAR_PROP_ANIMATOR_TIME "animator_time"
#define SCROLL_BAR_PROP_WHEEL_SCROLL "wheel_scroll"
#define SCROLL_BAR_PROP_SCROLL_DELTA "scroll_delta"
#define SCROLL_BAR_PROP_SCROLL_ROWS "scroll_rows"
#define SCROLL_BAR(widget) ((scroll_bar_t*)(scroll_bar_cast(WIDGET(widget))))

/*public for subclass and runtime type check*/
TK_EXTERN_VTABLE(scroll_bar_mobile);
TK_EXTERN_VTABLE(scroll_bar_desktop);

END_C_DECLS

#endif /*TK_SCROLL_BAR_H*/
