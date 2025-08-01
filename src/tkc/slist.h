﻿/**
 * File:   slist.h
 * Author: AWTK Develop Team
 * Brief:  single link list
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
 * 2019-01-11 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef TK_SLIST_H
#define TK_SLIST_H

#include "tkc/types_def.h"
#include "tkc/mem_allocator.h"

BEGIN_C_DECLS

typedef struct _slist_node_t slist_node_t;

struct _slist_node_t {
  slist_node_t* next;
  void* data;
};

/**
 * @class slist_t
 * 单向链表
 *
 * 用slist\_init初始化时，用slist\_deinit释放。如：
 *
 * ```c
 * slist_t slist;
 * slist_init(&slist, destroy, compare);
 * ...
 * slist_deinit(&slist);
 * ```
 *
 * 用slist\_create创建时，用slist\_destroy销毁。如：
 *
 * ```c
 * slist_t* slist = slist_create(destroy, compare);
 * ...
 * slist_destroy(slist);
 * ```
 *
 */
typedef struct _slist_t {
  /**
   * @property {slist_node_t*} first
   * @annotation ["readable"]
   * 首节点。
   */
  slist_node_t* first;

  /**
   * @property {slist_node_t*} last
   * @annotation ["readable"]
   * 尾节点。
   */
  slist_node_t* last;

  /**
   * @property {int32_t} size
   * @annotation ["readable"]
   * 元素个数。
   */
  int32_t size;

  /**
   * @property {tk_destroy_t} destroy
   * @annotation ["readable"]
   * 元素销毁函数。
   */
  tk_destroy_t destroy;

  /**
   * @property {tk_compare_t} compare
   * @annotation ["readable"]
   * 元素比较函数。
   */
  tk_compare_t compare;

  /**
   * @property {mem_allocator_t*} node_allocator
   * 节点内存分配器。
   */
  mem_allocator_t* node_allocator;

  /**
   * @property {bool_t} node_allocator_is_shared
   * 节点内存分配器是否共享。
   */
  bool_t node_allocator_is_shared;
} slist_t;

/**
 * @method slist_create
 * @annotation ["constructor"]
 * 创建slist对象
 * @param {tk_destroy_t} destroy 元素销毁函数。
 * @param {tk_compare_t} compare 元素比较函数。
 *
 * @return {slist_t*} 单向链表对象。
 */
slist_t* slist_create(tk_destroy_t destroy, tk_compare_t compare);

/**
 * @method slist_init
 * 初始化slist对象
 * @param {slist_t*} slist 单向链表对象。
 * @param {tk_destroy_t} destroy 元素销毁函数。
 * @param {tk_compare_t} compare 元素比较函数。
 *
 * @return {slist_t*} 单向链表对象。
 */
slist_t* slist_init(slist_t* slist, tk_destroy_t destroy, tk_compare_t compare);

/**
 * @method slist_find
 * 查找第一个满足条件的元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {void*} ctx 比较函数的上下文。
 *
 * @return {void*} 如果找到，返回满足条件的对象，否则返回NULL。
 */
void* slist_find(slist_t* slist, void* ctx);

/**
 * @method slist_find_ex
 * 查找第一个满足条件的元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {tk_compare_t} compare 元素比较函数。
 * @param {void*} ctx 比较函数的上下文。
 *
 * @return {void*} 返回节点。
 */
void* slist_find_ex(slist_t* slist, tk_compare_t compare, void* ctx);

/**
 * @method slist_remove
 * 删除第一个满足条件的元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {void*} ctx 比较函数的上下文。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_remove(slist_t* slist, void* ctx);

/**
 * @method slist_remove_ex
 * 删除满足条件的元素。
 * 备注：
 * 如果队列中符合条件的元素不足 remove_size，移除最后一个符合条件的元素后返回 RET_OK。
 * 如果队列中符合条件的元素大于 remove_size，在队列中移除 remove_size 个元素后返回 RET_OK。
 * remove_size 为负数则会移除所有符合条件的元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {tk_compare_t} compare 元素比较函数。
 * @param {void*} ctx 比较函数的上下文。
 * @param {int32_t} remove_size 删除个数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_remove_ex(slist_t* slist, tk_compare_t compare, void* ctx, int32_t remove_size);

#define slist_remove_with_compare(slist, ctx, compare, remove_size) \
  slist_remove_ex(slist, compare, ctx, remove_size)

/**
 * @method slist_remove_all
 * 删除全部元素。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_remove_all(slist_t* slist);

/**
 * @method slist_append
 * 在尾巴追加一个元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {void*} data 待追加的元素。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_append(slist_t* slist, void* data);

/**
 * @method slist_insert
 * 插入一个元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {uint32_t} index 位置序数。
 * @param {void*} data 待追加的元素。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_insert(slist_t* slist, uint32_t index, void* data);

/**
 * @method slist_prepend
 * 在头部追加一个元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {void*} data 待追加的元素。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_prepend(slist_t* slist, void* data);

/**
 * @method slist_foreach
 * 遍历元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {tk_visit_t} visit 遍历函数。
 * @param {void*} ctx 遍历函数的上下文。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_foreach(slist_t* slist, tk_visit_t visit, void* ctx);

/**
 * @method slist_get
 * 获取指定序数的元素。
 * @param {slist_t*} slist 单向链表对象。
 * @param {uint32_t} index 序数。
 *
 * @return {void*} 返回满足条件的对象，否则返回NULL。
 */
void* slist_get(slist_t* slist, uint32_t index);

/**
 * @method slist_tail_pop
 * 弹出最后一个元素。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {void*} 成功返回最后一个元素，失败返回NULL。
 */
void* slist_tail_pop(slist_t* slist);

/**
 * @method slist_head_pop
 * 弹出第一个元素。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {void*} 成功返回最后一个元素，失败返回NULL。
 */
void* slist_head_pop(slist_t* slist);

/**
 * @method slist_tail
 * 返回最后一个元素。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {void*} 成功返回最后一个元素，失败返回NULL。
 */
void* slist_tail(slist_t* slist);

/**
 * @method slist_head
 * 返回第一个元素。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {void*} 成功返回最后一个元素，失败返回NULL。
 */
void* slist_head(slist_t* slist);

/**
 * @method slist_is_empty
 * 列表是否为空。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {bool_t} 返回 TRUE 表示空列表，返回 FALSE 表示列表有数据。
 */
bool_t slist_is_empty(slist_t* slist);

/**
 * @method slist_size
 * 返回元素个数。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {int32_t} 返回元素个数。
 */
int32_t slist_size(slist_t* slist);

/**
 * @method slist_count
 * 返回满足条件元素的个数。
 * @param {slist_t*} slist 单向链表对象。
 * @param {void*} ctx 比较函数的上下文。
 *
 * @return {int32_t} 返回元素个数。
 */
int32_t slist_count(slist_t* slist, void* ctx);

/**
 * @method slist_reverse
 * 反转链表。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_reverse(slist_t* slist);

/**
 * @method slist_set_node_allocator
 * 设置节点内存分配器。
 * @param {slist_t*} slist 单向链表对象。
 * @param {mem_allocator_t*} allocator 内存分配器对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_set_node_allocator(slist_t* slist, mem_allocator_t* allocator);

/**
 * @method slist_set_shared_node_allocator
 * 设置共享节点内存分配器。
 * @param {slist_t*} slist 单向链表对象。
 * @param {mem_allocator_t*} allocator 内存分配器对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_set_shared_node_allocator(slist_t* slist, mem_allocator_t* allocator);

/**
 * @method slist_deinit
 * 清除单向链表中的元素。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_deinit(slist_t* slist);

/**
 * @method slist_destroy
 * 清除单向链表中的元素，并释放单向链表对象。
 * @param {slist_t*} slist 单向链表对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t slist_destroy(slist_t* slist);

END_C_DECLS

#endif /*TK_SLIST_H*/
