﻿/**
 * File:   main_loop_simple.h
 * Author: AWTK Develop Team
 * Brief:  a simple main loop
 *
 * Copyright (c) 2018 - 2025 Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * license file for more details.
 *
 */

/**
 * history:
 * ================================================================
 * 2018-05-17 li xianjing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_MAIN_LOOP_SIMPLE_H
#define TK_MAIN_LOOP_SIMPLE_H

#include "base/idle.h"
#include "base/timer.h"
#include "tkc/mutex.h"
#include "base/main_loop.h"
#include "base/event_queue.h"
#include "base/font_manager.h"
#include "base/window_manager.h"

BEGIN_C_DECLS

/**
 * @class main_loop_simple_t
 * @parent main_loop_t
 * 简单的主循环。
 */
struct _main_loop_simple_t;
typedef struct _main_loop_simple_t main_loop_simple_t;

typedef ret_t (*main_loop_dispatch_input_t)(main_loop_simple_t* loop);

struct _main_loop_simple_t {
  main_loop_t base;
  event_queue_t* queue;

  wh_t w;
  wh_t h;
  bool_t pressed;
  bool_t key_pressed;
  xy_t last_x;
  xy_t last_y;
  uint8_t last_key;
  tk_mutex_t* mutex;
  void* user1;
  void* user2;
  void* user3;
  void* user4;
  event_source_manager_t* event_source_manager;
  main_loop_dispatch_input_t dispatch_input;
};

/**
 * @method main_loop_simple_init
 * 初始化main_loop_simple_t对象。
 * @param {int} w 宽度。
 * @param {int} h 高度。
 * @param {main_loop_queue_event_t} queue_event 队列事件处理函数。
 * @param {main_loop_recv_event_t} recv_event 接收事件处理函数。
 *
 * @return {main_loop_simple_t*} 返回main_loop_simple_t对象。
 */
main_loop_simple_t* main_loop_simple_init(int w, int h, main_loop_queue_event_t queue_event,
                                          main_loop_recv_event_t recv_event);

/**
 * @method main_loop_simple_reset
 * 销毁main_loop_simple_t对象。
 * @param {main_loop_simple_t*} loop main_loop_simple_t对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t main_loop_simple_reset(main_loop_simple_t* loop);

/**
 * @method main_loop_post_key_event
 * 发送按键事件。
 * @param {main_loop_t*} l 主循环对象。
 * @param {bool_t} pressed 是否按下。
 * @param {uint8_t} key 按键。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 
*/
ret_t main_loop_post_key_event(main_loop_t* l, bool_t pressed, uint8_t key);

/**
 * @method main_loop_post_pointer_event
 * 发送指针事件。
 * @param {main_loop_t*} l 主循环对象。
 * @param {bool_t} pressed 是否按下。
 * @param {xy_t} x x坐标。
 * @param {xy_t} y y坐标。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t main_loop_post_pointer_event(main_loop_t* l, bool_t pressed, xy_t x, xy_t y);

/**
 * @method main_loop_post_touch_event
 * 发送触摸事件。
 * @param {main_loop_t*} l 主循环对象。
 * @param {event_type_t} event_type 事件类型(EVT_TOUCH_DOWN/EVT_TOUCH_UP/EVT_TOUCH_MOVE)。
 * @param {xy_t} x x坐标。
 * @param {xy_t} y y坐标。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t main_loop_post_touch_event(main_loop_t* l, event_type_t event_type, xy_t x, xy_t y);

/**
 * @method main_loop_post_multi_gesture_event
 * 发送多点触控事件。
 * @param {main_loop_t*} l 主循环对象。
 * @param {multi_gesture_event_t*} event 事件。
 * 
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t main_loop_post_multi_gesture_event(main_loop_t* l, multi_gesture_event_t* event);

#ifndef MAIN_LOOP_QUEUE_SIZE
#define MAIN_LOOP_QUEUE_SIZE 20
#endif /*MAIN_LOOP_QUEUE_SIZE*/

END_C_DECLS

#endif /*TK_MAIN_LOOP_SIMPLE_H*/
