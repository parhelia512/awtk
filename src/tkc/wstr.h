﻿/**
 * File:   wstr.h
 * Author: AWTK Develop Team
 * Brief:  wide char string
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
 * 2018-01-28 Li XianJing <xianjimli@hotmail.com> adapt from uclib
 *
 */

#ifndef WSTR_H
#define WSTR_H

#include "tkc/value.h"
#include "tkc/types_def.h"

BEGIN_C_DECLS

/**
 * @class wstr_t
 * 可变长度的宽字符字符串。
 *
 * 示例：
 *
 * ```c
 *  wstr_t s;
 *  wstr_init(&s, 0);
 *
 *  wstr_append(&s, L"abc");
 *  wstr_append(&s, L"123");
 *
 *  wstr_reset(&s);
 * ```
 * > 先调wstr\_init进行初始化，最后调用wstr\_reset释放内存。
 *
 */
typedef struct _wstr_t {
  /**
   * @property {uint32_t} size
   * @annotation ["readable"]
   * 长度。
   */
  uint32_t size;
  /**
   * @property {uint32_t} capacity
   * @annotation ["readable"]
   * 容量。
   */
  uint32_t capacity;
  /**
   * @property {wchar_t*} str
   * @annotation ["readable"]
   * 字符串。
   */
  wchar_t* str;

  /*private*/
  bool_t extendable;
} wstr_t;

/**
 * @method wstr_create
 * 创建str对象。
 * 备注：最后调用wstr\_destroy释放内存
 * @annotation ["constructor"]
 * @param {uint32_t} capacity 初始容量。
 *
 * @return {wstr_t*} str对象。
 */
wstr_t* wstr_create(uint32_t capacity);

/**
 * @method wstr_destroy
 * 销毁str对象。
 * @param {wstr_t*} str str对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_destroy(wstr_t* str);

/**
 * @method wstr_init
 * 初始化字符串对象。
 * 备注：最后调用wstr\_reset释放内存
 * @annotation ["constructor"]
 * @param {wstr_t*} str str对象。
 * @param {uint32_t} capacity 初始容量。
 *
 * @return {wstr_t*} str对象本身。
 */
wstr_t* wstr_init(wstr_t* str, uint32_t capacity);

/**
 * @method wstr_extend
 * 扩展字符串到指定的容量。
 * @param {wstr_t*} str wstr对象。
 * @param {uint32_t} capacity 新的容量。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_extend(wstr_t* str, uint32_t capacity);

/**
 * @method wstr_shrink
 * 如果字符串长度大于指定长度，收缩字符串到指定的长度。
 * @param {wstr_t*} str wstr对象。
 * @param {uint32_t} size 新的长度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_shrink(wstr_t* str, uint32_t size);

/**
 * @method wstr_attach
 * 通过附加到一个buff来初始化str。 
 * >可以避免str动态分配内存，同时也不会自动扩展内存，使用完成后无需调用str_reset。
 *
 *```c
 * wstr_t s;
 * wchar_t buff[32];
 * wstr_attach(&s, buff, ARRAY_SIZE(buff));
 * wstr_set(&s, L"abc");
 * wstr_append(&s, L"123");
 *```
 *
 * @annotation ["constructor"]
 * @param {wstr_t*} str str对象。
 * @param {wchar_t*} buff 缓冲区。
 * @param {uint32_t} capacity 初始容量。
 *
 * @return {wstr_t*} str对象本身。
 */
wstr_t* wstr_attach(wstr_t* str, wchar_t* buff, uint32_t capacity);

/**
 * @method wstr_set
 * 设置字符串。
 * @param {wstr_t*} str str对象。
 * @param {const wchar_t*} text 要设置的字符串。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_set(wstr_t* str, const wchar_t* text);

/**
 * @method wstr_set_with_len
 * 设置字符串。
 * @param {wstr_t*} str str对象。
 * @param {const wchar_t*} text 要设置的字符串。
 * @param {uint32_t} len 字符串长度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_set_with_len(wstr_t* str, const wchar_t* text, uint32_t len);

/**
 * @method wstr_clear
 * 清除字符串内容。
 * @param {wstr_t*} str str对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_clear(wstr_t* str);

/**
 * @method wstr_set_utf8
 * 设置UTF8字符串。
 * @param {wstr_t*} str str对象。
 * @param {const char*} text 要设置的字符串。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_set_utf8(wstr_t* str, const char* text);

/**
 * @method wstr_set_utf8_with_len
 * 设置UTF8字符串。
 * @param {wstr_t*} str str对象。
 * @param {const char*} text 要设置的字符串。
 * @param {uint32_t} len 长度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_set_utf8_with_len(wstr_t* str, const char* text, uint32_t len);

/**
 * @method wstr_get_utf8
 * 获取UTF8字符串。
 * @param {wstr_t*} str str对象。
 * @param {char*} text 返回的字符串。
 * @param {uint32_t} size text最大长度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_get_utf8(wstr_t* str, char* text, uint32_t size);

/**
 * @method wstr_remove
 * 删除指定范围的字符。
 * @param {wstr_t*} str str对象。
 * @param {uint32_t} offset 指定的位置。
 * @param {uint32_t} nr 要删除的字符数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_remove(wstr_t* str, uint32_t offset, uint32_t nr);

/**
 * @method wstr_insert
 * 在指定位置插入字符串。
 * @param {wstr_t*} str str对象。
 * @param {uint32_t} offset 指定的位置。
 * @param {const wchar_t*} text 待插入的文本。
 * @param {uint32_t} nr 要插入的字符数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_insert(wstr_t* str, uint32_t offset, const wchar_t* text, uint32_t nr);

/**
 * @method wstr_append
 * 追加字符串。
 * @param {wstr_t*} str str对象。
 * @param {const wchar_t*} text 要追加的字符串。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_append(wstr_t* str, const wchar_t* text);

/**
 * @method wstr_append_utf8
 * 追加UTF8字符串。
 * @param {wstr_t*} str str对象。
 * @param {const char*} text 要设置的字符串。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_append_utf8(wstr_t* str, const char* text);

/**
 * @method wstr_append_more
 * 追加多个字符串。以NULL结束。
 *
 * 示例：
 *
 * ```c
 *  wstr_t s;
 *  wstr_init(&s, 0);
 *
 *  wstr_append_more(&s, L"abc", L"123", NULL);
 *  log_debug("%s\n", s.str);
 *
 *  wstr_reset(&s);
 * ```
 * @param {wstr_t*} str str对象。
 * @param {const wchar_t*} text 要追加的字符串。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_append_more(wstr_t* str, const wchar_t* text, ...);

/**
 * @method wstr_modify_wchar
 * 修改字符。
 * @param {wstr_t*} str str对象。
 * @param {uint32_t} pos 修改的位置。
 * @param {wchar_t} c 修改的字符。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_modify_wchar(wstr_t* str, uint32_t pos, wchar_t c);

/**
 * @method wstr_append_with_len
 * 追加字符串。
 * @param {wstr_t*} str str对象。
 * @param {const wchar_t*} text 要追加的字符串。
 * @param {uint32_t} len 字符串长度。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_append_with_len(wstr_t* str, const wchar_t* text, uint32_t len);

/**
 * @method wstr_push
 * 追加一个字符。
 * @param {wstr_t*} str str对象。
 * @param {const wchar_t} c 字符。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_push(wstr_t* str, const wchar_t c);

/**
 * @method wstr_pop
 * 删除尾部字符。
 * @param {wstr_t*} str str对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_pop(wstr_t* str);

/**
 * @method wstr_push_int
 * 追加一个整数。
 * @param {wstr_t*} str str对象。
 * @param {const char*} format 格式(用于snprintf格式化数值)
 * @param {int32_t} value 数值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_push_int(wstr_t* str, const char* format, int32_t value);

/**
 * @method wstr_eq
 * 判断两个字符串是否相等。
 * @param {wstr_t*} str str对象。
 * @param {const wchar_t*} text 待比较的字符串。
 *
 * @return {bool_t} 返回是否相等。
 */
bool_t wstr_eq(wstr_t* str, const wchar_t* text);

/**
 * @method wstr_equal
 * 判断两个字符是否相同。
 * @param {wstr_t*} str str对象。
 * @param {wstr_t*} other str对象。
 *
 * @return {bool_t} 返回TRUE表示相同，否则表示不同。
 */
bool_t wstr_equal(wstr_t* str, wstr_t* other);

/**
 * @method wstr_from_int
 * 用整数初始化字符串。
 * @param {wstr_t*} str str对象。
 * @param {int32_t} v 整数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_from_int(wstr_t* str, int32_t v);

/**
 * @method wstr_from_int64
 * 用整数初始化字符串。
 * @param {wstr_t*} str str对象。
 * @param {int64_t} v 整数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_from_int64(wstr_t* str, int64_t v);

/**
 * @method wstr_append_int
 * 追加整数到字符串。
 * @param {wstr_t*} str str对象。
 * @param {int32_t} v 整数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_append_int(wstr_t* str, int32_t v);

/**
 * @method wstr_from_float
 * 用浮点数初始化字符串。
 * @param {wstr_t*} str str对象。
 * @param {double} v 浮点数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_from_float(wstr_t* str, double v);

/**
 * @method wstr_from_value
 * 用value初始化字符串。
 * @param {wstr_t*} str str对象。
 * @param {const value_t*} v value。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_from_value(wstr_t* str, const value_t* v);

/**
 * @method wstr_to_int
 * 将字符串转成整数。
 * @param {wstr_t*} str str对象。
 * @param {int32_t*} v 用于返回整数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_to_int(wstr_t* str, int32_t* v);

/**
 * @method wstr_to_int64
 * 将字符串转成整数。
 * @param {wstr_t*} str str对象。
 * @param {int64_t*} v 用于返回整数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_to_int64(wstr_t* str, int64_t* v);

/**
 * @method wstr_to_float
 * 将字符串转成浮点数。
 * @param {wstr_t*} str str对象。
 * @param {double*} v 用于返回浮点数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_to_float(wstr_t* str, double* v);

/**
 * @method wstr_add_float
 * 将字符串转成浮点数，加上delta，再转换回来。
 * @param {wstr_t*} str str对象。
 * @param {double} delta 要加上的值。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_add_float(wstr_t* str, double delta);

/**
 * @method wstr_trim_float_zero
 * 去掉浮点数小数点尾部的零。
 * @param {wstr_t*} str str对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_trim_float_zero(wstr_t* str);

/**
 * @method wstr_normalize_newline
 * 规范化换行符。
 * @param {wstr_t*} str str对象。
 * @param {wchar_t} newline 换行符。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_normalize_newline(wstr_t* str, wchar_t newline);

/**
 * @method wstr_count_char
 * 统计指定字符的个数。
 * @param {wstr_t*} str str对象。
 * @param {wchar_t} c 字符。
 *
 * @return {uint32_t} 返回指定字符的个数。
 */
uint32_t wstr_count_char(wstr_t* str, wchar_t c);

/**
 * @method wstr_reset
 * 重置字符串为空。
 * @param {wstr_t*} str str对象。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t wstr_reset(wstr_t* str);

#define wcs_dup wcsdup
#define wcs_len wcslen
#define wcs_case_cmp tk_wstricmp
#define wcs_cmp wcscmp
#define wcs_ncpy wcsncpy
#define wcs_cpy wcscpy
#define wcs_chr wcschr

#ifdef WITH_WCSXXX
#ifndef WITH_WASM
wchar_t* wcsdup(const wchar_t* s);
#endif /*WITH_WASM*/
#endif /*WITH_WCSXXX*/

#define WSTR_DESTROY(str) \
  do {                    \
    if (str != NULL) {    \
      wstr_destroy(str);  \
      str = NULL;         \
    }                     \
  } while (0)

END_C_DECLS

#endif /*WSTR_H*/
