﻿/**
 * File:   utils.h
 * Author: AWTK Develop Team
 * Brief:  utils struct and utils functions.
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
 * 2018-02-21 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#ifndef WITH_WASM
#include "tkc/thread.h"
#endif /*WITH_WASM*/

#include "tkc/fs.h"
#include "tkc/path.h"
#include "tkc/mem.h"
#include "tkc/utf8.h"
#include "tkc/wstr.h"
#include "tkc/utils.h"
#include "tkc/object.h"
#include "tkc/named_value.h"
#include "tkc/data_reader_factory.h"
#include "tkc/data_writer_factory.h"
#include "tkc/data_reader_mem.h"
#include "tkc/object_hash.h"
#include "tkc/object_array.h"
#include "conf_io/conf_json.h"

#define IS_ADDRESS_ALIGN_4(addr) !((((size_t)(addr)) & 0x3) | 0x0)

#ifndef WITH_WASM
static uint64_t s_ui_thread_id = 0;

ret_t tk_set_ui_thread(uint64_t ui_thread_id) {
  s_ui_thread_id = ui_thread_id;

  return RET_OK;
}

bool_t tk_is_ui_thread(void) {
  return s_ui_thread_id == (uint64_t)tk_thread_self();
}
#endif /*WITH_WASM*/

const char* tk_skip_to_num(const char* str) {
  const char* p = str;
  return_value_if_fail(str != NULL, NULL);

  while (*p && (*p < '0' || *p > '9')) p++;

  return p;
}

bool_t tk_atob(const char* str) {
  if (TK_STR_IS_EMPTY(str) || *str == 'f' || *str == 'F' || tk_str_eq(str, "0")) {
    return FALSE;
  }

  return TRUE;
}

#ifdef HAS_NO_LIBC
static long tk_strtol_internal(const char* str, const char** end, int base) {
  int i = 0;
  long n = 0;
  char c = 0;
  return_value_if_fail(str != NULL && (base == 10 || base == 8 || base == 16), 0);

  if (base == 10) {
    for (i = 0; str[i] && i < 10; i++) {
      c = str[i];

      if (c < '0' || c > '9') {
        break;
      }

      n = n * base + c - '0';
    }
  } else if (base == 8) {
    for (i = 0; str[i] && i < 10; i++) {
      c = str[i];

      if (c < '0' || c > '7') {
        break;
      }

      n = n * base + c - '0';
    }
  } else if (base == 16) {
    for (i = 0; str[i] && i < 10; i++) {
      c = str[i];

      if ((c >= '0' && c <= '9')) {
        c -= '0';
      } else if (c >= 'a' && c <= 'f') {
        c = c - 'a' + 10;
      } else if (c >= 'A' && c <= 'F') {
        c = c - 'A' + 10;
      } else {
        break;
      }

      n = n * base + c;
    }
  }

  if (end != NULL) {
    *end = str + i;
  }

  return n;
}

long tk_strtol(const char* str, const char** end, int base) {
  long n = 0;
  int neg = 0;
  return_value_if_fail(str != NULL, 0);

  while (*str == ' ' || *str == '\t') str++;

  if (*str == '+' || *str == '-') {
    neg = *str == '-';
    str++;
  }

  n = tk_strtol_internal(str, end, base);

  return neg ? -n : n;
}

double tk_atof(const char* str) {
  int n = 0;
  uint32_t zero = 0;
  unsigned int f = 0;
  int neg = 0;
  double result = 0;
  const char* p = NULL;
  return_value_if_fail(str != NULL, 0);

  if (str[0] == '+' || str[0] == '-') {
    neg = str[0] == '-';
    str++;
  }

  n = tk_strtol_internal(str, &p, 10);

  if (p != NULL && *p == '.') {
    p++;
    while (*p && *p == '0') {
      p++;
      zero++;
    }
    f = tk_strtol_internal(p, NULL, 10);
  }

  result = f;
  while (result >= 1) {
    result = result / 10;
  }
  while (zero > 0) {
    result = result / 10;
    zero--;
  }

  result = n + result;

  return neg ? -result : result;
}

static const char* tk_itoa_simple(char* str, int len, int64_t n, const char** end) {
  int i = 0;
  int need_len = 0;
  int64_t value = n;

  return_value_if_fail(str != NULL && len > 2, NULL);

  if (n == 0) {
    str[0] = '0';
    str[1] = '\0';

    if (end != NULL) {
      *end = str + 1;
    }

    return str;
  }

  if (n < 0) {
    n = -n;
    str[0] = '-';
    need_len++;
  }

  value = n;
  while (value > 0) {
    value = value / 10;
    need_len++;
  }

  need_len++; /*for null char*/
  return_value_if_fail(len > (need_len), NULL);

  i = need_len - 2;
  while (n > 0) {
    str[i--] = (n % 10) + '0';
    n = n / 10;
  }
  str[need_len - 1] = '\0';

  if (end != NULL) {
    *end = str + need_len - 1;
  }

  return str;
}

const char* tk_itoa(char* str, int len, int n) {
  return tk_itoa_simple(str, len, n, NULL);
}

const char* tk_lltoa(char* str, int len, uint64_t n) {
  return tk_itoa_simple(str, len, n, NULL);
}

#else
double tk_atof(const char* str) {
  return_value_if_fail(str != NULL, 0);

  return atof(str);
}

int32_t tk_strtoi(const char* str, const char** end, int base) {
  long ret = tk_strtol(str, end, base);
  if (ret > INT32_MAX) {
    ret = INT32_MAX;
  } else if (ret < INT32_MIN) {
    ret = INT32_MIN;
  }
  return (int32_t)ret;
}

long tk_strtol(const char* str, const char** end, int base) {
  return_value_if_fail(str != NULL, 0);

  return strtol(str, (char**)end, base);
}

int64_t tk_strtoll(const char* str, const char** end, int base) {
  return strtoll(str, (char**)end, base);
}

uint64_t tk_strtoull(const char* str, const char** end, int base) {
  return strtoull(str, (char**)end, base);
}

const char* tk_itoa(char* str, int len, int n) {
  return_value_if_fail(str != NULL, NULL);

  tk_snprintf(str, len, "%d", n);

  return str;
}

const char* tk_lltoa(char* str, int len, int64_t n) {
  return_value_if_fail(str != NULL, NULL);

  tk_snprintf(str, len, "%" PRId64, n);

  return str;
}
#endif /*HAS_NO_LIBC*/

#define IS_HEX_NUM(s) (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
#define IS_BIN_NUM(s) (s[0] == '0' && (s[1] == 'b' || s[1] == 'B'))

int32_t tk_atoi(const char* str) {
  return_value_if_fail(str != NULL, 0);
  if (IS_HEX_NUM(str)) {
    return tk_strtoi(str + 2, NULL, 16);
  } else if (str[0] == '#') {
    return tk_strtoi(str + 1, NULL, 16);
  } else if (IS_BIN_NUM(str)) {
    return tk_strtoi(str + 2, NULL, 2);
  } else {
    return tk_strtoi(str, NULL, 10);
  }
}

int64_t tk_atol(const char* str) {
  return_value_if_fail(str != NULL, 0);
  if (IS_HEX_NUM(str)) {
    return tk_strtoll(str + 2, NULL, 16);
  } else if (str[0] == '#') {
    return tk_strtoll(str + 1, NULL, 16);
  } else if (IS_BIN_NUM(str)) {
    return tk_strtoll(str + 2, NULL, 2);
  } else {
    return tk_strtoll(str, NULL, 10);
  }
}

uint64_t tk_atoul(const char* str) {
  return_value_if_fail(str != NULL, 0);
  if (IS_HEX_NUM(str)) {
    return tk_strtoull(str + 2, NULL, 16);
  } else if (str[0] == '#') {
    return tk_strtoull(str + 1, NULL, 16);
  } else if (IS_BIN_NUM(str)) {
    return tk_strtoull(str + 2, NULL, 2);
  } else {
    return tk_strtoull(str, NULL, 10);
  }
}

const char* tk_ftoa(char* str, int len, double value) {
  tk_snprintf(str, len, "%lf", value);

  return str;
}

char* tk_strcpy(char* dst, const char* src) {
  return_value_if_fail(dst != NULL && src != NULL, NULL);
  if (dst != src) {
    return strcpy(dst, src);
  } else {
    return dst;
  }
}

char* tk_strncpy(char* dst, const char* src, size_t len) {
  return_value_if_fail(dst != NULL && src != NULL, NULL);
  if (dst != src) {
    strncpy(dst, src, len);
    dst[len] = '\0';
  }
  return dst;
}

char* tk_strncpy_s(char* dst, size_t dst_len, const char* src, size_t src_len) {
  size_t len = 0;
  return_value_if_fail(dst != NULL && src != NULL && dst_len > 0, NULL);

  len = tk_min(dst_len - 1, src_len);

  return tk_strncpy(dst, src, len);
}

char* tk_strndup(const char* str, uint32_t len) {
  char* s = NULL;
  return_value_if_fail(str != NULL, NULL);

  s = (char*)TKMEM_ALLOC(len + 1);
  if (s != NULL) {
    memcpy(s, str, len);
    s[len] = '\0';
  }

  return s;
}

void* tk_memdup(const void* data, uint32_t len) {
  void* s = NULL;
  return_value_if_fail(data != NULL, NULL);

  s = TKMEM_ALLOC(len + 1);
  if (s != NULL) {
    memcpy(s, data, len);
    ((char*)s)[len] = '\0';
  }

  return s;
}

char* tk_strdup(const char* str) {
  return_value_if_fail(str != NULL, NULL);

  return tk_strndup(str, tk_strlen(str));
}

wchar_t* tk_wstrdup(const wchar_t* str) {
  uint32_t size = 0;
  wchar_t* new_str = NULL;
  return_value_if_fail(str != NULL, NULL);
  size = wcslen(str) + 1;

  new_str = TKMEM_ALLOC(size * sizeof(wchar_t));
  return_value_if_fail(new_str != NULL, NULL);
  memcpy(new_str, str, size * sizeof(wchar_t));

  return new_str;
}

uint16_t* tk_memset16(uint16_t* buff, uint16_t val, uint32_t size) {
  uint32_t n = 0;
  uint16_t* p = buff;
  uint8_t* pb = (uint8_t*)buff;

  return_value_if_fail(buff != NULL, NULL);

  while ((size_t)pb % 4 != 0 && size > 0) {
    *p = val;

    p++;
    size--;
    pb += 2;
  }

  n = size / 8; /*16bytes*/
  if (n > 0) {
    uint32_t* p32 = NULL;
    uint32_t data = val | (val << 16);

    while (n > 0) {
      p32 = (uint32_t*)pb;

      p32[0] = data;
      p32[1] = data;
      p32[2] = data;
      p32[3] = data;

      n--;
      pb += 16;
    }
  }

  n = size % 8;
  if (n > 0) {
    p = (uint16_t*)pb;
    while (n > 0) {
      *p = val;
      p++;
      n--;
    }
  }

  return buff;
}

uint32_t* tk_memset24(uint32_t* buff, void* val, uint32_t size) {
  uint32_t n = 0;
  uint32_t bytes = size * 3;
  uint8_t* pb = (uint8_t*)buff;
  uint8_t* src = (uint8_t*)val;

  while ((size_t)pb % 4 != 0 && size > 0) {
    pb[0] = src[0];
    pb[1] = src[1];
    pb[2] = src[2];

    pb += 3;
    size--;
  }

  bytes = size * 3;
  n = bytes / 12;

  if (n > 0) {
    uint32_t* p = NULL;
    uint32_t data0 = src[0] | src[1] << 8 | src[2] << 16 | src[0] << 24;
    uint32_t data1 = src[1] | src[2] << 8 | src[0] << 16 | src[1] << 24;
    uint32_t data2 = src[2] | src[0] << 8 | src[1] << 16 | src[2] << 24;

    while (n > 0) {
      p = (uint32_t*)pb;
      p[0] = data0;
      p[1] = data1;
      p[2] = data2;
      pb += 12;
      n--;
    }
  }

  bytes = bytes % 12;
  while (bytes > 0) {
    pb[0] = src[0];
    pb[1] = src[1];
    pb[2] = src[2];
    pb += 3;
    bytes -= 3;
  }

  return buff;
}

uint32_t* tk_memset32(uint32_t* buff, uint32_t val, uint32_t size) {
  uint32_t* p = buff;
  return_value_if_fail(buff != NULL, NULL);

  while (size-- > 0) {
    *p++ = val;
  }

  return buff;
}

uint16_t* tk_memcpy16(uint16_t* dst, uint16_t* src, uint32_t size) {
  uint16_t* d = dst;
  uint16_t* s = src;
  return_value_if_fail(dst != NULL && src != NULL, NULL);

  while (size-- > 0) {
    *d++ = *s++;
  }

  return dst;
}

uint32_t* tk_memcpy32(uint32_t* dst, uint32_t* src, uint32_t size) {
  uint32_t* d = dst;
  uint32_t* s = src;
  return_value_if_fail(dst != NULL && src != NULL, NULL);

  while (size-- > 0) {
    *d++ = *s++;
  }

  return dst;
}

void* tk_pixel_copy(void* dst, const void* src, uint32_t size, uint8_t bpp) {
  return_value_if_fail(dst != NULL && src != NULL, NULL);

#ifdef HAS_FAST_MEMCPY
  memcpy(dst, src, size * bpp);
#else
  if (bpp == 2) {
    tk_memcpy16((uint16_t*)dst, (uint16_t*)src, size);
  } else if (bpp == 4) {
    tk_memcpy32((uint32_t*)dst, (uint32_t*)src, size);
  } else {
    memcpy(dst, src, size * bpp);
  }
#endif /*HAS_FAST_MEMCPY*/

  return dst;
}

#if defined(LINUX) || defined(APPLE) || defined(HAS_STDIO) || defined(WINDOWS)
#include <stdio.h>
#else
extern int vsscanf(const char* s, const char* format, va_list arg);
extern int vsnprintf(char* str, size_t size, const char* format, va_list ap);
#endif /*LINUX || APPLE || HAS_STDIO || WINDOWS*/

int tk_snprintf(char* str, size_t size, const char* format, ...) {
  int ret = 0;
  va_list va;
  return_value_if_fail(str != NULL && format != NULL, 0);

  va_start(va, format);
  ret = tk_vsnprintf(str, size, format, va);
  va_end(va);

  return ret;
}

int tk_vsnprintf(char* str, size_t size, const char* format, va_list ap) {
  return_value_if_fail(str != NULL && format != NULL, 0);

  return vsnprintf(str, size, format, ap);
}

ret_t filename_to_name_ex(const char* filename, char* str, uint32_t size, bool_t remove_extname) {
  return path_basename_ex(filename, remove_extname, str, size);
}

ret_t filename_to_name(const char* filename, char* str, uint32_t size) {
  return filename_to_name_ex(filename, str, size, TRUE);
}

#define INCLUDE_XML "<?include"
#define TAG_PROPERTY "property"
#define CHAR_DOUBLE_QUOTE '\"'
#define CHAR_SINGLE_QUOTE '\''

typedef enum _xml_property_close_state_t {
  XML_PROPERTY_CLOSE_STATE_CLOSE = 0x0,
  XML_PROPERTY_CLOSE_STATE_OPEN_PROPERTY,
  XML_PROPERTY_CLOSE_STATE_OPEN_DOUBLE_QUOTE,
  XML_PROPERTY_CLOSE_STATE_OPEN_SINGLE_QUOTE,
} xml_property_close_state_t;

static xml_property_close_state_t xml_property_get_close_state(const char* start, const char* end) {
  const char* tmp = start;
  xml_property_close_state_t close_state = XML_PROPERTY_CLOSE_STATE_CLOSE;

  while (tmp != end) {
    if (*tmp == CHAR_DOUBLE_QUOTE) {
      if (close_state == XML_PROPERTY_CLOSE_STATE_OPEN_DOUBLE_QUOTE) {
        close_state = XML_PROPERTY_CLOSE_STATE_CLOSE;
      } else if (close_state == XML_PROPERTY_CLOSE_STATE_CLOSE) {
        close_state = XML_PROPERTY_CLOSE_STATE_OPEN_DOUBLE_QUOTE;
      }
    } else if (*tmp == CHAR_SINGLE_QUOTE) {
      if (close_state == XML_PROPERTY_CLOSE_STATE_OPEN_SINGLE_QUOTE) {
        close_state = XML_PROPERTY_CLOSE_STATE_CLOSE;
      } else if (close_state == XML_PROPERTY_CLOSE_STATE_CLOSE) {
        close_state = XML_PROPERTY_CLOSE_STATE_OPEN_SINGLE_QUOTE;
      }
    }
    tmp++;
  }
  return close_state;
}

ret_t tk_str_append(char* str, uint32_t max_len, const char* s) {
  uint32_t len = 0;
  uint32_t org_len = 0;
  return_value_if_fail(str != NULL && s != NULL, RET_BAD_PARAMS);

  len = tk_strlen(s);
  org_len = tk_strlen(str);

  return_value_if_fail(max_len > (len + org_len), RET_FAIL);
  memcpy(str + org_len, s, len);
  str[org_len + len] = '\0';

  return RET_OK;
}

int32_t tk_strcmp(const char* a, const char* b) {
  if (a == b) {
    return 0;
  }

  if (a == NULL) {
    return -1;
  }

  if (b == NULL) {
    return 1;
  }

  return strcmp(a, b);
}

int32_t tk_stricmp(const char* a, const char* b) {
  if (a == b) {
    return 0;
  }

  if (a == NULL) {
    return -1;
  }

  if (b == NULL) {
    return 1;
  }

  return strcasecmp(a, b);
}

int32_t tk_wstrcmp(const wchar_t* a, const wchar_t* b) {
  if (a == b) {
    return 0;
  }

  if (a == NULL) {
    return -1;
  }

  if (b == NULL) {
    return 1;
  }

  return wcscmp(a, b);
}

int32_t tk_wstricmp(const wchar_t* a, const wchar_t* b) {
  if (a == b) {
    return 0;
  }

  if (a == NULL) {
    return -1;
  }

  if (b == NULL) {
    return 1;
  }

  while (towlower(*a) == towlower(*b)) {
    if (*a == 0) {
      return 0;
    }
    a++;
    b++;
  }

  return tolower(*a) - tolower(*b);
}

char* tk_str_copy(char* dst, const char* src) {
  if (src != NULL) {
    uint32_t size = tk_strlen(src) + 1;
    if (dst != NULL) {
      if (dst <= src && src <= dst + tk_strlen(dst)) {
        if (dst != src) {
          memcpy(dst, src, size);
        }
      } else {
        char* str = TKMEM_REALLOCT(char, dst, size);
        return_value_if_fail(str != NULL, dst);
        memcpy(str, src, size);
        dst = str;
      }
    } else {
      char* str = tk_strndup(src, size - 1);
      return_value_if_fail(str != NULL, dst);
      dst = str;
    }
  } else {
    if (dst != NULL) {
      *dst = '\0';
    }
  }

  return dst;
}

int tk_watoi_n(const wchar_t* str, uint32_t len) {
  char num[TK_NUM_MAX_LEN + 1] = {0};
  return_value_if_fail(str != NULL, 0);

  memset(num, 0x00, sizeof(num));
  tk_utf8_from_utf16_ex(str, len, num, TK_NUM_MAX_LEN);

  return tk_atoi(num);
}

int tk_watoi(const wchar_t* str) {
  return_value_if_fail(str != NULL, 0);

  return tk_watoi_n(str, wcslen(str));
}

bool_t tk_watob(const wchar_t* str) {
  if (TK_STR_IS_EMPTY(str) || *str == 'f' || *str == 'F' || tk_wstr_eq(str, L"0")) {
    return FALSE;
  }

  return TRUE;
}

double tk_watof(const wchar_t* str) {
  char num[TK_NUM_MAX_LEN + 1] = {0};
  return_value_if_fail(str != NULL, 0);

  tk_utf8_from_utf16(str, num, TK_NUM_MAX_LEN);

  return tk_atof(num);
}

ret_t default_destroy(void* data) {
  TKMEM_FREE(data);

  return RET_OK;
}

ret_t dummy_destroy(void* data) {
  return RET_OK;
}

int pointer_compare(const void* a, const void* b) {
  return ((const char*)a - (const char*)b);
}

int compare_always_equal(const void* a, const void* b) {
  return 0;
}

ret_t tk_replace_locale(const char* name, char out[TK_NAME_LEN + 1], const char* locale) {
  char* d = NULL;
  char* p = NULL;
  int32_t len = 0;
  const char* s = NULL;
  return_value_if_fail(tk_strlen(name) < TK_NAME_LEN, RET_BAD_PARAMS);
  return_value_if_fail(tk_strlen(locale) <= tk_strlen(TK_LOCALE_MAGIC), RET_BAD_PARAMS);
  return_value_if_fail(name != NULL && out != NULL && locale != NULL, RET_BAD_PARAMS);

  d = out;
  s = name;
  p = strstr(name, TK_LOCALE_MAGIC);
  return_value_if_fail(p != NULL, RET_BAD_PARAMS);

  len = p - s;
  memcpy(d, s, len);
  d += len;

  len = tk_strlen(locale);
  memcpy(d, locale, len);

  d += len;
  tk_strcpy(d, p + tk_strlen(TK_LOCALE_MAGIC));

  return RET_OK;
}

bool_t tk_is_valid_name(const char* name) {
  const char* p = name;
  while (*p) {
    if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') ||
        *p == '_') {
      p++;
    } else {
      return FALSE;
    }
  }

  return TRUE;
}

bool_t tk_str_start_with(const char* str, const char* prefix) {
  return_value_if_fail(str != NULL && prefix != NULL, FALSE);

  return strncmp(str, prefix, tk_strlen(prefix)) == 0;
}

bool_t tk_str_end_with(const char* str, const char* appendix) {
  uint32_t len_str = 0;
  uint32_t len_appendix = 0;
  return_value_if_fail(str != NULL && appendix != NULL, FALSE);

  len_str = tk_strlen(str);
  len_appendix = tk_strlen(appendix);

  if (len_str < len_appendix) {
    return FALSE;
  } else {
    return strncmp(str + len_str - len_appendix, appendix, len_appendix) == 0;
  }
}

bool_t tk_str_case_start_with(const char* str, const char* prefix) {
  return_value_if_fail(str != NULL && prefix != NULL, FALSE);

  return strncasecmp(str, prefix, tk_strlen(prefix)) == 0;
}

bool_t tk_str_case_end_with(const char* str, const char* appendix) {
  uint32_t len_str = 0;
  uint32_t len_appendix = 0;
  return_value_if_fail(str != NULL && appendix != NULL, FALSE);

  len_str = tk_strlen(str);
  len_appendix = tk_strlen(appendix);

  if (len_str < len_appendix) {
    return FALSE;
  } else {
    return strncasecmp(str + len_str - len_appendix, appendix, len_appendix) == 0;
  }
}

const char* tk_under_score_to_camel(const char* name, char* out, uint32_t max_out_size) {
  uint32_t i = 0;
  const char* s = name;
  return_value_if_fail(name != NULL && out != NULL && max_out_size > 0, NULL);

  while (*s && i < max_out_size) {
    if (*s == '_') {
      s++;
      if (*s != '\0') {
        out[i++] = toupper(*s);
      } else {
        break;
      }
    } else {
      out[i++] = *s;
    }
    s++;
  }
  out[i] = '\0';

  return out;
}

int32_t tk_pointer_to_int(const void* p) {
  return (int32_t)(intptr_t)p;
}

void* tk_pointer_from_int(int32_t v) {
  return (void*)(intptr_t)v;
}

uint64_t tk_pointer_to_long(const void* p) {
  return (uint64_t)(uintptr_t)p;
}

void* tk_pointer_from_long(uint64_t v) {
  ENSURE(v <= UINTPTR_MAX);
  return (void*)(uintptr_t)v;
}

char* tk_str_toupper(char* str) {
  char* p = str;
  return_value_if_fail(str != NULL, NULL);

  while (*p) {
    *p = toupper(*p);
    p++;
  }

  return str;
}

char* tk_str_totitle(char* str) {
  char* p = str;
  char* prev = str;
  return_value_if_fail(str != NULL, NULL);

  while (*p) {
    if (tk_isalpha(*p)) {
      if (p == str || (!tk_isalpha(*prev) && !tk_isdigit(*prev))) {
        *p = toupper(*p);
      }
    }
    prev = p;
    p++;
  }

  return str;
}

char* tk_str_tolower(char* str) {
  char* p = str;
  return_value_if_fail(str != NULL, NULL);

  while (*p) {
    *p = tolower(*p);
    p++;
  }

  return str;
}

const char* tk_normalize_key_name(const char* name, char fixed_name[TK_NAME_LEN + 1]) {
  uint32_t len = 0;
  return_value_if_fail(name != NULL && fixed_name != NULL, NULL);

  len = tk_strlen(name);
  tk_strncpy(fixed_name, name, TK_NAME_LEN);

  if (len > 1) {
    tk_str_toupper(fixed_name);
  }

  return fixed_name;
}

uint32_t tk_strlen(const char* str) {
  if (str == NULL || *str == '\0') {
    return 0;
  }

  return strlen(str);
}

wchar_t* tk_wstr_dup_utf8(const char* str) {
  int32_t len = 0;
  int32_t size = 0;
  wchar_t* wstr = NULL;
  return_value_if_fail(str != NULL, NULL);

  len = tk_strlen(str) + 1;
  size = len * sizeof(wchar_t);
  wstr = TKMEM_ALLOC(size);
  return_value_if_fail(wstr != NULL, NULL);
  memset(wstr, 0x00, size);

  tk_utf8_to_utf16(str, wstr, len);

  return wstr;
}

uint32_t tk_wstr_count_c(const wchar_t* str, wchar_t c) {
  uint32_t nr = 0;
  const wchar_t* p = str;
  return_value_if_fail(p != NULL, nr);

  while (*p) {
    if (*p == c) {
      nr++;
    }
    p++;
  }

  return nr;
}

uint32_t tk_count_char(const char* str, char c) {
  uint32_t nr = 0;
  const char* p = str;
  return_value_if_fail(p != NULL, nr);

  while (*p) {
    if (*p == c) {
      nr++;
    }
    p++;
  }

  return nr;
}

ret_t tk_wstr_select_word(const wchar_t* str, uint32_t len, uint32_t index, int32_t* left,
                          int32_t* right) {
  int32_t i = 0;
  const wchar_t* no_start_symbols = L",.?!)>:;，。？！》）：；";

  return_value_if_fail(str != NULL, RET_BAD_PARAMS);
  return_value_if_fail(index <= len, RET_BAD_PARAMS);
  return_value_if_fail(left != NULL && right != NULL, RET_BAD_PARAMS);

  i = (int32_t)index;
  while (str != NULL && i > 0) {
    --i;

    if (i >= 0) {
      if (wcschr(no_start_symbols, str[i]) != NULL) {
        break;
      } else if (str[i] == '\r' || str[i] == '\n') {
        break;
      } else if (tk_isspace(str[i])) {
        break;
      } else if (!tk_isspace(str[i + 1]) && wcschr(no_start_symbols, str[i + 1]) == NULL &&
                 ((tk_isalpha(str[i]) && !tk_isalpha(str[i + 1])) ||
                  (!tk_isalpha(str[i]) && tk_isalpha(str[i + 1])))) {
        break;
      } else if ((tk_isdigit(str[i]) && !tk_isdigit(str[i + 1])) ||
                 (!tk_isdigit(str[i]) && tk_isdigit(str[i + 1]))) {
        break;
      }
    }
  }

  if (i <= 0) {
    *left = 0;
  } else {
    *left = i + 1;
  }

  i = (int32_t)index;
  while (str != NULL && i >= 0 && i < len) {
    ++i;

    if (i >= 0 && i <= len) {
      if (wcschr(no_start_symbols, str[i - 1]) != NULL) {
        *right = i - 1;
        break;
      } else if (wcschr(no_start_symbols, str[i]) != NULL) {
        *right = i;
        break;
      } else if (str[i - 1] == '\r' || str[i] == '\n') {
        *right = i;
        break;
      } else if (tk_isspace(str[i])) {
        *right = i;
        break;
      } else if (tk_isspace(str[i - 1])) {
        *right = i - 1;
        break;
      } else if ((tk_isalpha(str[i]) && !tk_isalpha(str[i - 1])) ||
                 (!tk_isalpha(str[i]) && tk_isalpha(str[i - 1]))) {
        *right = i;
        break;
      } else if ((tk_isdigit(str[i]) && !tk_isdigit(str[i - 1])) ||
                 (!tk_isdigit(str[i]) && tk_isdigit(str[i - 1]))) {
        *right = i;
        break;
      }
    }
  }

  if (i >= len) {
    *right = len > 0 ? len : 0;
  }

  return RET_OK;
}

ret_t image_region_parse(uint32_t img_w, uint32_t img_h, const char* region, rect_t* r) {
  return_value_if_fail(r != NULL && region != NULL, RET_BAD_PARAMS);
  if (*region == '#') {
    region++;
  }

  if (strncmp(region, "xywh(", 5) == 0) {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    ENSURE(tk_sscanf(region, "xywh(%d,%d,%d,%d)", &x, &y, &w, &h) == 4);

    *r = rect_init(x, y, w, h);
    return RET_OK;
  } else if (strncmp(region, "grid(", 5) == 0) {
    int rows = 0;
    int cols = 0;
    int row = 0;
    int col = 0;
    int tile_w = 0;
    int tile_h = 0;
    ENSURE(tk_sscanf(region, "grid(%d,%d,%d,%d)", &rows, &cols, &row, &col) == 4);
    return_value_if_fail(rows > 0 && cols > 0 && row >= 0 && col >= 0, RET_FAIL);
    return_value_if_fail(rows > row && cols > col, RET_FAIL);

    tile_w = img_w / cols;
    tile_h = img_h / rows;

    *r = rect_init(col * tile_w, row * tile_h, tile_w, tile_h);
    return RET_OK;
  }

  return RET_FAIL;
}

#ifdef WITH_DATA_READER_WRITER
ret_t data_url_copy(const char* dst_url, const char* src_url) {
  ret_t ret = RET_OK;
  return_value_if_fail(dst_url != NULL && src_url != NULL, RET_BAD_PARAMS);

  data_reader_t* reader = data_reader_factory_create_reader(data_reader_factory(), src_url);
  if (reader != NULL) {
    uint32_t size = data_reader_get_size(reader);
    if (size > 0) {
      data_writer_t* writer = data_writer_factory_create_writer(data_writer_factory(), dst_url);
      if (writer != NULL) {
        void* buff = TKMEM_CALLOC(1, size + 1);
        if (buff != NULL) {
          int32_t rsize = data_reader_read(reader, 0, buff, size);
          assert(rsize == size);
          rsize = data_writer_write(writer, 0, buff, rsize);
          assert(rsize == size);
          TKMEM_FREE(buff);
          log_debug("copy: %s=>%s\n", src_url, dst_url);
        } else {
          ret = RET_FAIL;
        }
        data_writer_destroy(writer);
      } else {
        ret = RET_FAIL;
        log_debug("open dst(%s) failed\n", dst_url);
      }
    } else {
      log_debug("open src(%s) failed\n", src_url);
    }
    data_reader_destroy(reader);
  }

  return ret;
}
#endif /*WITH_DATA_READER_WRITER*/

static void tk_quick_sort_impl(void** array, size_t left, size_t right, tk_compare_t cmp) {
  size_t save_left = left;
  size_t save_right = right;
  void* x = array[left];

  while (left < right) {
    while (cmp(array[right], x) >= 0 && left < right) right--;
    if (left != right) {
      array[left] = array[right];
      left++;
    }

    while (cmp(array[left], x) <= 0 && left < right) left++;
    if (left != right) {
      array[right] = array[left];
      right--;
    }
  }
  array[left] = x;

  if (save_left < left) {
    tk_quick_sort_impl(array, save_left, left - 1, cmp);
  }

  if (save_right > left) {
    tk_quick_sort_impl(array, left + 1, save_right, cmp);
  }

  return;
}

ret_t tk_qsort(void** array, size_t nr, tk_compare_t cmp) {
  ret_t ret = RET_OK;

  return_value_if_fail(array != NULL && cmp != NULL, RET_BAD_PARAMS);

  if (nr > 1) {
    tk_quick_sort_impl(array, 0, nr - 1, cmp);
  }

  return ret;
}

const char* tk_strrstr(const char* str, const char* substr) {
  char c = 0;
  uint32_t len = 0;
  const char* p = NULL;
  const char* end = NULL;
  return_value_if_fail(str != NULL && substr != NULL, NULL);

  c = *substr;
  len = tk_strlen(substr);
  end = str + tk_strlen(str) - 1;

  for (p = end; p >= str; p--) {
    if (*p == c) {
      if (strncmp(p, substr, len) == 0) {
        return p;
      }
    }
  }

  return NULL;
}

bool_t tk_int_is_in_array(int32_t v, const int32_t* array, uint32_t array_size) {
  uint32_t i = 0;
  return_value_if_fail(array != NULL && array_size > 0, FALSE);

  for (i = 0; i < array_size; i++) {
    if (v == array[i]) {
      return TRUE;
    }
  }

  return FALSE;
}

bool_t tk_str_is_in_array(const char* str, const char** str_array, uint32_t array_size) {
  uint32_t i = 0;
  return_value_if_fail(str != NULL && str_array != NULL && array_size > 0, FALSE);

  for (i = 0; i < array_size; i++) {
    if (tk_str_eq(str, str_array[i])) {
      return TRUE;
    }
  }

  return FALSE;
}

void* tk_memcpy_by_align_4(void* dst_align_4, const void* src_align_4, uint32_t len) {
#define MEMCPY_TINY_MAX_LENGTH 64
  if (dst_align_4 != NULL && src_align_4 != NULL && len > 0) {
    if (len < MEMCPY_TINY_MAX_LENGTH) {
      register unsigned char* dd = (unsigned char*)dst_align_4 + len;
      register const unsigned char* ss = (const unsigned char*)src_align_4 + len;

      assert(IS_ADDRESS_ALIGN_4(dst_align_4));
      assert(IS_ADDRESS_ALIGN_4(src_align_4));

      switch (len) {
        case 64:
          *((int*)(dd - 64)) = *((int*)(ss - 64));
        case 60:
          *((int*)(dd - 60)) = *((int*)(ss - 60));
        case 56:
          *((int*)(dd - 56)) = *((int*)(ss - 56));
        case 52:
          *((int*)(dd - 52)) = *((int*)(ss - 52));
        case 48:
          *((int*)(dd - 48)) = *((int*)(ss - 48));
        case 44:
          *((int*)(dd - 44)) = *((int*)(ss - 44));
        case 40:
          *((int*)(dd - 40)) = *((int*)(ss - 40));
        case 36:
          *((int*)(dd - 36)) = *((int*)(ss - 36));
        case 32:
          *((int*)(dd - 32)) = *((int*)(ss - 32));
        case 28:
          *((int*)(dd - 28)) = *((int*)(ss - 28));
        case 24:
          *((int*)(dd - 24)) = *((int*)(ss - 24));
        case 20:
          *((int*)(dd - 20)) = *((int*)(ss - 20));
        case 16:
          *((int*)(dd - 16)) = *((int*)(ss - 16));
        case 12:
          *((int*)(dd - 12)) = *((int*)(ss - 12));
        case 8:
          *((int*)(dd - 8)) = *((int*)(ss - 8));
        case 4:
          *((int*)(dd - 4)) = *((int*)(ss - 4));
          break;
        case 63:
          *((int*)(dd - 63)) = *((int*)(ss - 63));
        case 59:
          *((int*)(dd - 59)) = *((int*)(ss - 59));
        case 55:
          *((int*)(dd - 55)) = *((int*)(ss - 55));
        case 51:
          *((int*)(dd - 51)) = *((int*)(ss - 51));
        case 47:
          *((int*)(dd - 47)) = *((int*)(ss - 47));
        case 43:
          *((int*)(dd - 43)) = *((int*)(ss - 43));
        case 39:
          *((int*)(dd - 39)) = *((int*)(ss - 39));
        case 35:
          *((int*)(dd - 35)) = *((int*)(ss - 35));
        case 31:
          *((int*)(dd - 31)) = *((int*)(ss - 31));
        case 27:
          *((int*)(dd - 27)) = *((int*)(ss - 27));
        case 23:
          *((int*)(dd - 23)) = *((int*)(ss - 23));
        case 19:
          *((int*)(dd - 19)) = *((int*)(ss - 19));
        case 15:
          *((int*)(dd - 15)) = *((int*)(ss - 15));
        case 11:
          *((int*)(dd - 11)) = *((int*)(ss - 11));
        case 7:
          *((int*)(dd - 7)) = *((int*)(ss - 7));
        case 3:
          *((short*)(dd - 3)) = *((short*)(ss - 3));
          dd[-1] = ss[-1];
          break;
        case 62:
          *((int*)(dd - 62)) = *((int*)(ss - 62));
        case 58:
          *((int*)(dd - 58)) = *((int*)(ss - 58));
        case 54:
          *((int*)(dd - 54)) = *((int*)(ss - 54));
        case 50:
          *((int*)(dd - 50)) = *((int*)(ss - 50));
        case 46:
          *((int*)(dd - 46)) = *((int*)(ss - 46));
        case 42:
          *((int*)(dd - 42)) = *((int*)(ss - 42));
        case 38:
          *((int*)(dd - 38)) = *((int*)(ss - 38));
        case 34:
          *((int*)(dd - 34)) = *((int*)(ss - 34));
        case 30:
          *((int*)(dd - 30)) = *((int*)(ss - 30));
        case 26:
          *((int*)(dd - 26)) = *((int*)(ss - 26));
        case 22:
          *((int*)(dd - 22)) = *((int*)(ss - 22));
        case 18:
          *((int*)(dd - 18)) = *((int*)(ss - 18));
        case 14:
          *((int*)(dd - 14)) = *((int*)(ss - 14));
        case 10:
          *((int*)(dd - 10)) = *((int*)(ss - 10));
        case 6:
          *((int*)(dd - 6)) = *((int*)(ss - 6));
        case 2:
          *((short*)(dd - 2)) = *((short*)(ss - 2));
          break;
        case 61:
          *((int*)(dd - 61)) = *((int*)(ss - 61));
        case 57:
          *((int*)(dd - 57)) = *((int*)(ss - 57));
        case 53:
          *((int*)(dd - 53)) = *((int*)(ss - 53));
        case 49:
          *((int*)(dd - 49)) = *((int*)(ss - 49));
        case 45:
          *((int*)(dd - 45)) = *((int*)(ss - 45));
        case 41:
          *((int*)(dd - 41)) = *((int*)(ss - 41));
        case 37:
          *((int*)(dd - 37)) = *((int*)(ss - 37));
        case 33:
          *((int*)(dd - 33)) = *((int*)(ss - 33));
        case 29:
          *((int*)(dd - 29)) = *((int*)(ss - 29));
        case 25:
          *((int*)(dd - 25)) = *((int*)(ss - 25));
        case 21:
          *((int*)(dd - 21)) = *((int*)(ss - 21));
        case 17:
          *((int*)(dd - 17)) = *((int*)(ss - 17));
        case 13:
          *((int*)(dd - 13)) = *((int*)(ss - 13));
        case 9:
          *((int*)(dd - 9)) = *((int*)(ss - 9));
        case 5:
          *((int*)(dd - 5)) = *((int*)(ss - 5));
        case 1:
          dd[-1] = ss[-1];
          break;
        case 0:
        default:
          break;
      }
      return dd;
    }
    return memcpy(dst_align_4, src_align_4, len);
  } else {
    return dst_align_4;
  }
}

void* tk_memcpy(void* dst, const void* src, uint32_t len) {
  size_t offset_src_address = ((size_t)src) & 0x3;
  size_t offset_dst_address = ((size_t)dst) & 0x3;
  return_value_if_fail(dst != NULL && src != NULL, NULL);
  if (offset_src_address == offset_dst_address) {
    uint32_t i = 0;
    uint8_t* d = dst;
    uint8_t* s = (uint8_t*)src;
    int32_t curr_len = len;
    uint32_t offset = 0x4 - offset_src_address;
    if (offset_src_address != 0) {
      for (i = 0; i < offset && i < len; i++) {
        d[i] = s[i];
      }
      d += offset;
      s += offset;
      curr_len -= offset;
    }
    if (curr_len > 0) {
      return tk_memcpy_by_align_4((void*)d, (const void*)s, curr_len);
    } else {
      return dst;
    }
  } else {
    return memcpy(dst, src, len);
  }
}

bool_t tk_wild_card_match(const char* pattern, const char* str) {
  const char* p_str = str;
  const char* p_pattern = pattern;
  return_value_if_fail(pattern != NULL && str != NULL, FALSE);

  while (*p_pattern && *p_str) {
    char c = *p_pattern;
    if (c == '*') {
      c = p_pattern[1];
      if (c == '\0') {
        break;
      }
      while (*p_str) {
        if (c == *p_str) {
          break;
        } else {
          p_str++;
        }
      }
    } else if (c == '?') {
      p_str++;
    } else {
      if (c != *p_str) {
        return FALSE;
      }
      p_str++;
    }
    p_pattern++;
  }

  return *p_pattern == '*' || *p_str == *p_pattern;
}

float_t tk_eval_ratio_or_px(const char* expr, int32_t value) {
  const char* p = NULL;
  return_value_if_fail(expr != NULL, 0);

  p = strrchr(expr, 'p');
  if (p == NULL) {
    p = strrchr(expr, 'P');
  }

  if (p == NULL) {
    float_t ratio = tk_atof(expr);
    if (ratio > 1 && strchr(expr, '%') != NULL) {
      /*ex: 50% */
      return (value * ratio) / 100.0f;
    } else {
      /*ex: 0.5 */
      /*ex: 0.5% (兼容以前的处理)*/
      return value * ratio;
    }
  } else {
    /*ex: 100px */
    return tk_atoi(expr);
  }
}

char* tk_replace_char(char* str, char from, char to) {
  char* p = str;
  return_value_if_fail(str != NULL, NULL);

  while (*p) {
    if (*p == from) {
      *p = to;
    }
    p++;
  }

  return str;
}

uint32_t tk_strnlen(const char* str, uint32_t maxlen) {
  const char* s;
  return_value_if_fail(str != NULL, 0);

  for (s = str; maxlen-- && *s != '\0'; ++s);
  return s - str;
}

static ret_t xml_file_expand_subfilename_get(const char** p, const char* filename,
                                             char subfilename[MAX_PATH + 1]) {
  str_t s;
  const char* sp = *p;
  ret_t ret = RET_FAIL;
  return_value_if_fail(sp != NULL && filename != NULL && subfilename != NULL, RET_BAD_PARAMS);

  str_init(&s, 1024);

  /*<include filename="subfilename">*/
  while (*sp != '\"' && *sp != '\0') {
    sp++;
  }
  goto_error_if_fail(*sp == '\"');
  sp++;
  while (*sp != '\"' && *sp != '\0') {
    str_append_char(&s, *sp++);
  }
  goto_error_if_fail(*sp == '\"');
  while (*sp != '>' && *sp != '\0') {
    sp++;
  }
  goto_error_if_fail(*sp == '>');
  sp++;

  path_replace_basename(subfilename, MAX_PATH, filename, s.str);

  *p = sp;
  ret = RET_OK;
error:
  str_reset(&s);
  return ret;
}

ret_t xml_file_expand(const char* filename, str_t* s, const char* data) {
  str_t ss;
  char subfilename[MAX_PATH + 1];

  const char* start = data;
  const char* p = strstr(start, INCLUDE_XML);

  str_init(&ss, 1024);
  while (p != NULL) {
    /* 过滤在属性中的 INCLUDE_XML(目前只过滤在 \' 或 \" 作用域下的文件扩展, 对于property属性中的扩展不再做限制) */
    xml_property_close_state_t close_state = xml_property_get_close_state(start, p);
    if (close_state == XML_PROPERTY_CLOSE_STATE_CLOSE) {
      str_set(&ss, "");
      str_append_with_len(s, start, p - start);
      return_value_if_fail(xml_file_expand_subfilename_get(&p, filename, subfilename) == RET_OK,
                           RET_FAIL);
      xml_file_expand_read(subfilename, &ss);
      str_append(s, ss.str);
    } else {
      int size = 0;
      char* str_end = NULL;
      char* include_string_end = strstr(p, "?>");
      ENSURE(include_string_end);
      if (close_state == XML_PROPERTY_CLOSE_STATE_OPEN_PROPERTY) {
        str_end = TAG_PROPERTY;
        size = tk_strlen(TAG_PROPERTY);
      } else if (close_state == XML_PROPERTY_CLOSE_STATE_OPEN_SINGLE_QUOTE) {
        size = 1;
        str_end = "\'";
      } else if (close_state == XML_PROPERTY_CLOSE_STATE_OPEN_DOUBLE_QUOTE) {
        size = 1;
        str_end = "\"";
      }
      if (str_end == NULL) {
        log_error("do not find close property string !");
      } else {
        p = strstr(include_string_end, str_end) + size;
        str_append_with_len(s, start, p - start);
      }
    }

    start = p;
    p = strstr(start, INCLUDE_XML);
  }

  str_append(s, start);
  str_reset(&ss);

  return RET_OK;
}

ret_t xml_file_expand_read(const char* filename, str_t* s) {
  uint32_t size = 0;
  char* buff = NULL;
  return_value_if_fail(filename != NULL && s != NULL, RET_BAD_PARAMS);

  str_set(s, "");
  buff = (char*)file_read(filename, &size);
  return_value_if_fail(buff != NULL, RET_FAIL);

  if (strstr(buff, INCLUDE_XML) != NULL) {
    xml_file_expand(filename, s, buff);
  } else {
    str_set_with_len(s, (const char*)buff, size);
  }
  TKMEM_FREE(buff);

  return RET_OK;
}

ret_t xml_file_expand_subfilenames_get(const char* filename, char*** subfilenames, uint32_t* size) {
  ret_t ret = RET_FAIL;
  uint32_t i = 0, buff_len = 0;
  char *buff = NULL, *start = NULL, *p = NULL;
  char subfilename[MAX_PATH + 1] = {0}, **tmp_subfilenames = NULL;
  return_value_if_fail(filename != NULL && subfilenames != NULL && size != NULL, RET_BAD_PARAMS);

  buff = file_read(filename, &buff_len);
  return_value_if_fail(buff != NULL, RET_FAIL);

  *size = 0;

  start = buff;
  p = strstr(start, INCLUDE_XML);
  while (p != NULL) {
    xml_property_close_state_t close_state = xml_property_get_close_state(start, p);
    if (close_state == XML_PROPERTY_CLOSE_STATE_CLOSE) {
      ret = xml_file_expand_subfilename_get((const char**)&p, filename, subfilename);
      goto_error_if_fail(ret == RET_OK);
      tmp_subfilenames = TKMEM_REALLOCT(char*, *subfilenames, (++(*size)));
      goto_error_if_fail(tmp_subfilenames != NULL);
      tmp_subfilenames[*size - 1] = tk_strdup(subfilename);
      *subfilenames = tmp_subfilenames;
    }
    start = p;
    p = strstr(start, INCLUDE_XML);
  }
  ret = RET_OK;

error:
  if (ret != RET_OK && *size > 0) {
    for (i = 0; i < *size; i++) {
      TKMEM_FREE(*subfilenames[i]);
    }
    *size = 0;
    TKMEM_FREE(*subfilenames);
  }
  TKMEM_FREE(buff);
  return ret;
}

char* file_read_as_unix_text(const char* filename, uint32_t* size) {
  str_t str;
  uint32_t s = 0;
  char* data = (char*)file_read(filename, &s);
  return_value_if_fail(data != NULL, NULL);

  str_attach_with_size(&str, data, s, s + 1);
  str_replace(&str, "\r\n", "\n");
  str_replace(&str, "\r", "\n");
  *size = str.size;
  str_reset(&str);

  return data;
}

static const char* s_ret_names[RET_MAX_NR] = {[RET_OK] = "RET_OK",
                                              [RET_OOM] = "RET_OOM",
                                              [RET_FAIL] = "RET_FAIL",
                                              [RET_NOT_IMPL] = "RET_NOT_IMPL",
                                              [RET_QUIT] = "RET_QUIT",
                                              [RET_FOUND] = "RET_FOUND",
                                              [RET_BUSY] = "RET_BUSY",
                                              [RET_REMOVE] = "RET_REMOVE",
                                              [RET_REPEAT] = "RET_REPEAT",
                                              [RET_NOT_FOUND] = "RET_NOT_FOUND",
                                              [RET_DONE] = "RET_DONE",
                                              [RET_STOP] = "RET_STOP",
                                              [RET_SKIP] = "RET_SKIP",
                                              [RET_CONTINUE] = "RET_CONTINUE",
                                              [RET_OBJECT_CHANGED] = "RET_OBJECT_CHANGED",
                                              [RET_ITEMS_CHANGED] = "RET_ITEMS_CHANGED",
                                              [RET_BAD_PARAMS] = "RET_BAD_PARAMS",
                                              [RET_TIMEOUT] = "RET_TIMEOUT",
                                              [RET_CRC] = "RET_CRC",
                                              [RET_IO] = "RET_IO",
                                              [RET_EOS] = "RET_EOS",
                                              [RET_NOT_MODIFIED] = "RET_NOT_MODIFIED",
                                              [RET_NO_PERMISSION] = "RET_NO_PERMISSION",
                                              [RET_INVALID_ADDR] = "RET_INVALID_ADDR",
                                              [RET_EXCEED_RANGE] = "RET_EXCEED_RANGE"};

ret_t ret_code_from_name(const char* name) {
  uint32_t i = 0;
  uint32_t n = ARRAY_SIZE(s_ret_names);

  for (i = 0; i < n; i++) {
    if (tk_str_ieq(name, s_ret_names[i])) {
      return (ret_t)i;
    }
  }

  return RET_MAX_NR;
}

const char* ret_code_to_name(ret_t ret) {
  if (ret < RET_MAX_NR) {
    return s_ret_names[ret];
  } else {
    return "";
  }
}

ret_t bits_stream_get(const uint8_t* buff, uint32_t size, uint32_t index, bool_t* value) {
  uint8_t v = 0;
  uint32_t offset = index % 8;
  uint32_t max_index = size * 8;
  return_value_if_fail(buff != NULL, RET_BAD_PARAMS);
  return_value_if_fail(value != NULL, RET_BAD_PARAMS);
  if (index >= max_index) {
    log_debug("bits_stream_get failed: %u >= %u\n", index, max_index);
    return RET_BAD_PARAMS;
  }
  return_value_if_fail(index < max_index, RET_BAD_PARAMS);

  v = buff[index >> 3];

  *value = TK_TEST_BIT(v, offset);

  return RET_OK;
}

ret_t bits_stream_set(uint8_t* buff, uint32_t size, uint32_t index, bool_t value) {
  uint8_t v = 0;
  uint32_t offset = index % 8;
  uint32_t max_index = size * 8;
  return_value_if_fail(buff != NULL, RET_BAD_PARAMS);
  if (index >= max_index) {
    log_debug("bits_stream_get failed: %u >= %u\n", index, max_index);
    return RET_BAD_PARAMS;
  }
  return_value_if_fail(index < max_index, RET_BAD_PARAMS);

  v = buff[index >> 3];
  if (value) {
    TK_SET_BIT(v, offset);
  } else {
    TK_CLEAR_BIT(v, offset);
  }
  buff[index >> 3] = v;

  return RET_OK;
}

char* tk_utf8_dup_wstr(const wchar_t* str) {
  str_t s;
  return_value_if_fail(str != NULL, NULL);
  str_init(&s, wcslen(str) * 4 + 1);
  str_from_wstr(&s, str);

  return s.str;
}

char** tk_to_utf8_argv(int argc, wchar_t** argv) {
  uint32_t i = 0;
  char** argv_utf8 = NULL;
  argv_utf8 = TKMEM_ALLOC(sizeof(char*) * argc);
  return_value_if_fail(argv_utf8 != NULL, NULL);

  for (i = 0; i < argc; i++) {
    argv_utf8[i] = tk_utf8_dup_wstr(argv[i]);
  }

  return argv_utf8;
}

ret_t tk_free_utf8_argv(int argc, char** argv) {
  uint32_t i = 0;
  return_value_if_fail(argv != NULL, RET_BAD_PARAMS);

  for (i = 0; i < argc; i++) {
    TKMEM_FREE(argv[i]);
  }
  TKMEM_FREE(argv);

  return RET_OK;
}

static const char* str_skip_xint(const char* str) {
  if (*str == '+' || *str == '-') {
    str++;
  }

  while (*str && tk_isxdigit(*str)) {
    str++;
  }

  return str;
}

static const char* str_skip_int(const char* str) {
  if (*str == '+' || *str == '-') {
    str++;
  }

  while (*str && tk_isdigit(*str)) {
    str++;
  }

  return str;
}

static const char* str_skip_float(const char* str) {
  if (*str == '+' || *str == '-') {
    str++;
  }

  while (*str && tk_isdigit(*str)) {
    str++;
  }

  if (*str == '.' || *str == 'e' || *str == 'E') {
    str++;
    if (*str == '+' || *str == '-') {
      str++;
    }
    while (*str && tk_isdigit(*str)) {
      str++;
    }
  }

  return str;
}

int tk_vsscanf_simple(const char* str, const char* format, va_list args) {
  int assignments = 0;

  while (*format && *str) {
    if (*format != '%') {
      if (*format != *str) break;

      format++;
      str++;
    } else {
      /* 跳过 % */
      format++;

      /*跳过空白字符*/
      while (isspace(*str)) str++;

      if (tk_isdigit(*format)) {
        /*%02x*/
        /*%02d*/
        int n = tk_atoi(format);
        char temp[256] = {0};
        const char* s = temp;
        tk_strncpy(temp, str, tk_min_int(n, sizeof(temp) - 1));

        while (tk_isdigit(*format)) format++;

        switch (*format) {
          case 'd': {
            int* p = va_arg(args, int*);
            *p = (int)tk_strtol(s, NULL, 10);
            assignments++;
            format++;
            break;
          }
          case 'x': {
            unsigned int* p = va_arg(args, unsigned int*);
            *p = (unsigned int)tk_strtol(s, NULL, 16);
            assignments++;
            format++;
            break;
          }
          case 'u': {
            unsigned int* p = va_arg(args, unsigned int*);
            *p = (unsigned int)tk_strtol(s, NULL, 10);
            assignments++;
            format++;
            break;
          }
          case 's': {
            char* p = va_arg(args, char*);
            tk_strcpy(p, temp);
            assignments++;
            format++;
            break;
          }
          default: {
            log_warn("tk_sscanf_simple: not support %s\n", format);
            format++;
            break;
          }
        }
        str += n;
        continue;
      }
      switch (*format) {
        case 'u': {
          unsigned int* p = va_arg(args, unsigned int*);
          *p = (uint32_t)tk_strtol(str, NULL, 10);
          str = str_skip_int(str);
          assignments++;
          format++;
          break;
        }
        case 'd': {
          int* p = va_arg(args, int*);
          *p = (int)tk_strtol(str, NULL, 10);
          str = str_skip_int(str);
          assignments++;
          format++;
          break;
        }
        case 'f': {
          float* p = va_arg(args, float*);
          *p = (float)tk_atof(str);
          str = str_skip_float(str);
          assignments++;
          format++;
          break;
        }
        case 'x': {
          unsigned int* p = va_arg(args, unsigned int*);
          *p = (unsigned int)tk_strtol(str, NULL, 16);
          str = str_skip_xint(str);
          assignments++;
          format++;
          break;
        }
        case 'p': {
          void** p = va_arg(args, void**);
          if (strncasecmp(str, "0x", 2) == 0) {
            str += 2;
          }
          *p = tk_pointer_from_long(tk_strtol(str, NULL, 16));
          str = str_skip_xint(str);
          assignments++;
          format++;
          break;
        }
        case 'l': {
          format++;
          if (*format == 'd') {
            long* p = va_arg(args, long*);
            *p = (long)tk_strtol(str, NULL, 10);
            str = str_skip_int(str);
            assignments++;
          } else if (*format == 'u') {
            unsigned long* p = va_arg(args, unsigned long*);
            *p = (unsigned long)tk_strtol(str, NULL, 10);
            str = str_skip_int(str);
            assignments++;
            assignments++;
          } else if (*format == 'f') {
            double* p = va_arg(args, double*);
            *p = (double)tk_atof(str);
            str = str_skip_float(str);
            assignments++;
          }
          format++;
          break;
        }
        default:
          break;
      }
    }
  }

  return assignments;
}

int tk_sscanf_simple(const char* str, const char* format, ...) {
  int ret = 0;
  va_list va;
  va_start(va, format);
  ret = tk_vsscanf_simple(str, format, va);
  va_end(va);

  return ret;
}

#ifdef HAS_NO_VSSCANF
int sscanf(const char* str, const char* format, ...) {
  int ret = 0;
  va_list va;
  va_start(va, format);
  ret = tk_vsscanf_simple(str, format, va);
  va_end(va);

  return ret;
}

int vsscanf(const char* str, const char* format, va_list args) {
  return tk_vsscanf_simple(str, format, args);
}

int tk_sscanf(const char* str, const char* format, ...) {
  int ret = 0;
  va_list va;
  va_start(va, format);
  ret = tk_vsscanf_simple(str, format, va);
  va_end(va);

  return ret;
}
#else
int tk_sscanf(const char* str, const char* format, ...) {
  int ret = 0;
  va_list va;
  va_start(va, format);
  ret = vsscanf(str, format, va);
  va_end(va);

  return ret;
}
#endif /*HAS_NO_VSSCANF*/

int32_t tk_levelize(const char* levels, int32_t value) {
  int32_t level = 0;
  int32_t start = 0;
  int32_t end = 0;
  const char* p = levels;
  return_value_if_fail(levels != NULL, 0);

  while (*p) {
    if (tk_sscanf(p, "%d-%d", &start, &end) == 2) {
      if (value < start) {
        break;
      }

      if (value >= start && value <= end) {
        return level;
      }
    }

    p = strchr(p, ';');
    if (p != NULL) {
      p = p + 1;
      level++;
    } else {
      break;
    }
  }

  return level;
}

static uint32_t tk_count_repeat_char(const char* str, char c) {
  uint32_t nr = 0;
  return_value_if_fail(str != NULL, 0);

  while (*str++ == c) {
    nr++;
    if (*str == '\0') {
      break;
    }
  }

  return nr;
}

ret_t tk_date_time_format_impl(const date_time_t* dt, const char* format, str_t* result,
                               tk_on_result_t translate_callback) {
  const char* p = format;
  return_value_if_fail(dt != NULL && format != NULL && result != NULL, RET_BAD_PARAMS);

  str_clear(result);

  while (*p) {
    int32_t repeat = tk_count_repeat_char(p, *p);

    switch (*p) {
      case 'Y': {
        if (repeat == 2) {
          str_append_format(result, 32, "%02d", dt->year % 100);
        } else {
          str_append_format(result, 32, "%d", dt->year);
        }
        break;
      }
      case 'M': {
        if (repeat == 2) {
          str_append_format(result, 32, "%02d", dt->month);
        } else if (repeat == 3) {
          static const char* const months[] = {"Jan", "Feb", "Mar",  "Apr", "May", "Jun",
                                               "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};
          if (dt->month - 1 < ARRAY_SIZE(months)) {
            const char* tr_str = months[dt->month - 1];
            if (translate_callback != NULL) {
              translate_callback((void*)(&tr_str), months[dt->month - 1]);
            }
            str_append(result, tr_str);
          }
        } else {
          str_append_format(result, 32, "%d", dt->month);
        }
        break;
      }
      case 'D': {
        if (repeat == 2) {
          str_append_format(result, 32, "%02d", dt->day);
        } else {
          str_append_format(result, 32, "%d", dt->day);
        }
        break;
      }
      case 'h': {
        if (repeat == 2) {
          str_append_format(result, 32, "%02d", dt->hour);
        } else {
          str_append_format(result, 32, "%d", dt->hour);
        }
        break;
      }
      case 'T': {
        const char* str = (dt->hour < 12) ? "AM" : "PM";
        const char* tr_str = str;
        if (translate_callback != NULL) {
          translate_callback((void*)(&tr_str), str);
        }
        str_append(result, tr_str);
        break;
      }
      case 'H': {
        if (repeat == 2) {
          if (dt->hour == 0) {
            str_append_format(result, 32, "%02d", 12);
          } else {
            str_append_format(result, 32, "%02d", ((dt->hour > 12) ? (dt->hour - 12) : (dt->hour)));
          }
        } else {
          if (dt->hour == 0) {
            str_append_format(result, 32, "%d", 12);
          } else {
            str_append_format(result, 32, "%d", ((dt->hour > 12) ? (dt->hour - 12) : (dt->hour)));
          }
        }
        break;
      }
      case 'm': {
        if (repeat == 2) {
          str_append_format(result, 32, "%02d", dt->minute);
        } else {
          str_append_format(result, 32, "%d", dt->minute);
        }
        break;
      }
      case 's': {
        if (repeat == 2) {
          str_append_format(result, 32, "%02d", dt->second);
        } else {
          str_append_format(result, 32, "%d", dt->second);
        }
        break;
      }
      case 'w': {
        str_append_format(result, 32, "%d", dt->wday);
        break;
      }
      case 'W': {
        static const char* const wdays[] = {
            "Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat",
        };
        if (dt->wday < ARRAY_SIZE(wdays)) {
          const char* tr_str = wdays[dt->wday];
          if (translate_callback != NULL) {
            translate_callback((void*)(&tr_str), wdays[dt->wday]);
          }
          str_append(result, tr_str);
        }
        break;
      }
      default: {
        str_append_with_len(result, p, repeat);
        break;
      }
    }
    p += repeat;
  }

  return RET_OK;
}

ret_t tk_date_time_format(uint64_t time, const char* format, str_t* result) {
  date_time_t dt;
  return_value_if_fail(format != NULL && result != NULL, RET_BAD_PARAMS);

  memset(&dt, 0x00, sizeof(dt));
  date_time_from_time(&dt, time);

  return tk_date_time_format_impl(&dt, format, result, NULL);
}

uint32_t tk_bits_to_bytes(uint32_t bits) {
  return (bits + 7) / 8;
}

ret_t tk_bits_data_from_bytes_data(uint8_t* bits, uint32_t bits_size, uint8_t* bytes,
                                   uint32_t bytes_size) {
  uint32_t i = 0;

  for (i = 0; i < bytes_size; i++) {
    bits_stream_set(bits, bits_size, i, bytes[i] != 0);
  }

  return RET_OK;
}

ret_t tk_bits_data_to_bytes_data(uint8_t* bits, uint32_t bits_size, uint8_t* bytes,
                                 uint32_t bytes_size) {
  uint32_t i = 0;
  for (i = 0; i < bytes_size; i++) {
    bool_t value = FALSE;

    if (bits_stream_get(bits, bits_size, i, &value) == RET_OK) {
      bytes[i] = value ? 1 : 0;
    }
  }

  return RET_OK;
}

uint32_t tk_size_of_basic_type(value_type_t type) {
  switch (type) {
    case VALUE_TYPE_INT8:
    case VALUE_TYPE_STRING:
    case VALUE_TYPE_BINARY:
    case VALUE_TYPE_UINT8:
    case VALUE_TYPE_BOOL:
      return 1;
    case VALUE_TYPE_INT16:
    case VALUE_TYPE_UINT16:
      return 2;
    case VALUE_TYPE_INT32:
    case VALUE_TYPE_UINT32:
    case VALUE_TYPE_FLOAT32:
      return 4;
    case VALUE_TYPE_INT64:
    case VALUE_TYPE_UINT64:
    case VALUE_TYPE_DOUBLE:
      return 8;
    default:
      break;
  }

  return 0;
}

uint8_t* tk_skip_to_offset(uint8_t* data, uint32_t size, value_type_t type, int16_t index) {
  uint32_t type_size = tk_size_of_basic_type(type);
  uint32_t offset = index * type_size;
  return_value_if_fail(data != NULL && type_size > 0, NULL);
  return_value_if_fail((offset + type_size) <= size, NULL);

  return data + offset;
}

ret_t tk_buffer_set_value(uint8_t* buffer, uint32_t size, value_type_t type, int16_t offset,
                          int16_t bit_offset, const value_t* value) {
  uint32_t type_size = tk_size_of_basic_type(type);
  uint8_t* data = tk_skip_to_offset(buffer, size, type, offset);
  return_value_if_fail(data != NULL, RET_BAD_PARAMS);

  if (bit_offset >= 0) {
    return_value_if_fail(bit_offset < type_size * 8, RET_BAD_PARAMS);
    return bits_stream_set(data, type_size, bit_offset, value_bool(value));
  }

  switch (type) {
    case VALUE_TYPE_INT8: {
      *data = value_int8(value);
      break;
    }
    case VALUE_TYPE_UINT8: {
      *data = value_uint8(value);
      break;
    }
    case VALUE_TYPE_INT16: {
      int16_t v = value_int16(value);
      memcpy(data, &v, sizeof(v));
      break;
    }
    case VALUE_TYPE_UINT16: {
      uint16_t v = value_uint16(value);
      memcpy(data, &v, sizeof(v));
      break;
    }
    case VALUE_TYPE_INT32: {
      int32_t v = value_int32(value);
      memcpy(data, &v, sizeof(v));
      break;
    }
    case VALUE_TYPE_UINT32: {
      uint32_t v = value_uint32(value);
      memcpy(data, &v, sizeof(v));
      break;
    }
    case VALUE_TYPE_INT64: {
      int64_t v = value_int64(value);
      memcpy(data, &v, sizeof(v));
      break;
    }
    case VALUE_TYPE_UINT64: {
      uint64_t v = value_uint64(value);
      memcpy(data, &v, sizeof(v));
      break;
    }
    case VALUE_TYPE_FLOAT32: {
      float v = value_float32(value);
      memcpy(data, &v, sizeof(v));
      break;
    }
    case VALUE_TYPE_DOUBLE: {
      double v = value_double(value);
      memcpy(data, &v, sizeof(v));
      break;
    }
    case VALUE_TYPE_STRING: {
      const char* str = value_str(value);
      int32_t dst_len = size - (data - buffer);
      int32_t src_len = tk_strlen(str) + 1;
      return_value_if_fail(str != NULL, RET_BAD_PARAMS);
      return_value_if_fail(dst_len >= src_len, RET_BAD_PARAMS);

      memcpy(data, str, src_len);
      break;
    }
    case VALUE_TYPE_BINARY: {
      binary_data_t* bdata = value_binary_data(value);
      int32_t dst_len = size - (data - buffer);
      return_value_if_fail(bdata != NULL, RET_BAD_PARAMS);
      return_value_if_fail(dst_len >= bdata->size, RET_BAD_PARAMS);
      memcpy(data, bdata->data, bdata->size);
      break;
    }
    default: {
      log_debug("tk_buffer_set_value: not support %d\n", type);
      return RET_NOT_IMPL;
    }
  }

  return RET_OK;
}

ret_t tk_buffer_get_value(uint8_t* buffer, uint32_t size, value_type_t type, int16_t offset,
                          int16_t bit_offset, value_t* value) {
  bool_t v = FALSE;
  uint32_t type_size = tk_size_of_basic_type(type);
  uint8_t* data = tk_skip_to_offset(buffer, size, type, offset);
  return_value_if_fail(data != NULL, RET_BAD_PARAMS);
  return_value_if_fail(type_size > 0, RET_BAD_PARAMS);

  if (bit_offset >= 0) {
    return_value_if_fail(bit_offset < type_size * 8, RET_BAD_PARAMS);
    bits_stream_get(data, type_size, bit_offset, &v);
    value_set_bool(value, v);

    return RET_OK;
  }

  switch (type) {
    case VALUE_TYPE_INT8: {
      value_set_int8(value, *data);
      break;
    }
    case VALUE_TYPE_UINT8: {
      value_set_uint8(value, *data);
      break;
    }
    case VALUE_TYPE_INT16: {
      int16_t v = 0;
      memcpy(&v, data, sizeof(v));
      value_set_int16(value, v);
      break;
    }
    case VALUE_TYPE_UINT16: {
      uint16_t v = 0;
      memcpy(&v, data, sizeof(v));
      value_set_uint16(value, v);
      break;
    }
    case VALUE_TYPE_INT32: {
      int32_t v = 0;
      memcpy(&v, data, sizeof(v));
      value_set_int32(value, v);
      break;
    }
    case VALUE_TYPE_UINT32: {
      uint32_t v = 0;
      memcpy(&v, data, sizeof(v));
      value_set_uint32(value, v);
      break;
    }
    case VALUE_TYPE_INT64: {
      int64_t v = 0;
      memcpy(&v, data, sizeof(v));
      value_set_int64(value, v);
      break;
    }
    case VALUE_TYPE_UINT64: {
      uint64_t v = 0;
      memcpy(&v, data, sizeof(v));
      value_set_uint64(value, v);
      break;
    }
    case VALUE_TYPE_FLOAT32: {
      float v = 0;
      memcpy(&v, data, sizeof(v));
      value_set_float32(value, v);
      break;
    }
    case VALUE_TYPE_DOUBLE: {
      double v = 0;
      memcpy(&v, data, sizeof(v));
      value_set_double(value, v);
      break;
    }
    case VALUE_TYPE_STRING: {
      value_set_str(value, (char*)data);
      break;
    }
    case VALUE_TYPE_BINARY: {
      uint32_t bsize = size - (data - buffer);
      value_set_binary_data(value, data, bsize);
      break;
    }
    default: {
      log_debug("tk_buffer_get_value: not support %d\n", type);
      return RET_NOT_IMPL;
    }
  }

  return RET_OK;
}

value_type_t tk_basic_type_from_name(const char* type) {
  if (strncasecmp(type, "int8", 4) == 0) {
    return VALUE_TYPE_INT8;
  } else if (strncasecmp(type, "bool", 4) == 0) {
    return VALUE_TYPE_BOOL;
  } else if (strncasecmp(type, "int16", 5) == 0) {
    return VALUE_TYPE_INT16;
  } else if (strncasecmp(type, "int32", 5) == 0) {
    return VALUE_TYPE_INT32;
  } else if (strncasecmp(type, "int64", 5) == 0) {
    return VALUE_TYPE_INT64;
  } else if (strncasecmp(type, "uint8", 5) == 0 || strncasecmp(type, "byte", 4) == 0) {
    return VALUE_TYPE_UINT8;
  } else if (strncasecmp(type, "uint16", 6) == 0 || strncasecmp(type, "word", 4) == 0) {
    return VALUE_TYPE_UINT16;
  } else if (strncasecmp(type, "uint32", 6) == 0 || strncasecmp(type, "dword", 5) == 0) {
    return VALUE_TYPE_UINT32;
  } else if (strncasecmp(type, "uint64", 6) == 0 || strncasecmp(type, "lword", 5) == 0) {
    return VALUE_TYPE_UINT64;
  } else if (strncasecmp(type, "float", 5) == 0) {
    return VALUE_TYPE_FLOAT32;
  } else if (strncasecmp(type, "double", 6) == 0) {
    return VALUE_TYPE_DOUBLE;
  } else if (strncasecmp(type, "str", 3) == 0) {
    return VALUE_TYPE_STRING;
  } else if (strncasecmp(type, "wstr", 4) == 0) {
    return VALUE_TYPE_WSTRING;
  } else if (strncasecmp(type, "bin", 3) == 0) {
    return VALUE_TYPE_BINARY;
  }

  return VALUE_TYPE_INVALID;
}

uint32_t tk_distance(int x1, int y1, int x2, int y2) {
  return (uint32_t)sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

double tk_value_to_angle(double value, double min, double max, double start_angle, double end_angle,
                         bool_t counter_clock_wise) {
  double offset_angle = 0;
  double range = max - min;
  double range_angle = 0;
  return_value_if_fail(range > 0, 0);

  start_angle = TK_D2R(start_angle);
  end_angle = TK_D2R(end_angle);
  range_angle = end_angle - start_angle;
  value = tk_clamp(value, min, max);
  offset_angle = (range_angle / range) * (value - min);

  if (counter_clock_wise) {
    return end_angle - offset_angle;
  } else {
    return start_angle + offset_angle;
  }
}

double tk_angle(int cx, int cy, int x, int y) {
  double angle = 0;
  double dx = x - cx;
  double dy = -(y - cy);

  if (dx == 0) {
    if (dy > 0) {
      angle = M_PI / 2;
    } else {
      angle = -M_PI / 2;
    }
  } else {
    angle = atan2(dy, dx);
  }

  if (angle < 0) {
    angle = angle + 2 * M_PI;
  }

  return angle;
}

const char* tk_skip_chars(const char* str, const char* chars) {
  return_value_if_fail(str != NULL && chars != NULL, str);

  while (*str && strchr(chars, *str) != NULL) {
    str++;
  }
  return str;
}

const char* tk_skip_to_chars(const char* str, const char* chars) {
  return_value_if_fail(str != NULL && chars != NULL, str);

  while (*str && strchr(chars, *str) == NULL) {
    str++;
  }
  return str;
}

static void merge(void* base, size_t size, tk_compare_t cmp, void* left, size_t leftSize,
                  void* right, size_t rightSize) {
  // 创建临时数组
  void* temp = TKMEM_ALLOC((leftSize + rightSize) * size);
  if (temp == NULL) {
    log_warn("Memory allocation failed\n");
    return;
  }

  size_t i = 0, j = 0, k = 0;

  // 归并两个子数组
  while (i < leftSize && j < rightSize) {
    if (cmp((char*)left + i * size, (char*)right + j * size) <= 0) {
      memcpy((char*)temp + k * size, (char*)left + i * size, size);
      i++;
    } else {
      memcpy((char*)temp + k * size, (char*)right + j * size, size);
      j++;
    }
    k++;
  }

  // 复制剩余的左边部分
  while (i < leftSize) {
    memcpy((char*)temp + k * size, (char*)left + i * size, size);
    i++;
    k++;
  }

  // 复制剩余的右边部分
  while (j < rightSize) {
    memcpy((char*)temp + k * size, (char*)right + j * size, size);
    j++;
    k++;
  }

  // 将排序后的内容复制回原数组
  memcpy(base, temp, (leftSize + rightSize) * size);
  TKMEM_FREE(temp);
}

static void tk_mergesort_impl(void* base, size_t nmemb, size_t size, tk_compare_t cmp) {
  if (nmemb < 2) return;  // 如果只有一个元素，不需要排序

  size_t mid = nmemb / 2;

  // 递归排序左半部分
  tk_mergesort_impl(base, mid, size, cmp);
  // 递归排序右半部分
  tk_mergesort_impl((char*)base + mid * size, nmemb - mid, size, cmp);

  // 归并排序的结果
  void* left = base;
  void* right = (char*)base + mid * size;

  // 计算左右部分的大小
  size_t leftSize = mid;
  size_t rightSize = nmemb - mid;

  // 合并两个已排序的部分
  merge(base, size, cmp, left, leftSize, right, rightSize);
}

ret_t tk_mergesort(void* base, size_t nmemb, size_t size, tk_compare_t cmp) {
  return_value_if_fail(cmp != NULL, RET_BAD_PARAMS);
  return_value_if_fail(base != NULL && nmemb != 0 && size != 0, RET_BAD_PARAMS);

  tk_mergesort_impl(base, nmemb, size, cmp);

  return RET_OK;
}

const char* tk_strs_bsearch(const char** strs, uint32_t nr, const char* str,
                            bool_t case_sensitive) {
  int32_t low = 0;
  int32_t mid = 0;
  int32_t high = 0;
  int32_t result = 0;
  const char* iter = NULL;
  tk_compare_t cmp = case_sensitive ? (tk_compare_t)strcmp : (tk_compare_t)strcasecmp;
  return_value_if_fail(strs != NULL && str != NULL, NULL);

  if (nr == 0) {
    return NULL;
  }

  high = nr - 1;
  while (low <= high) {
    mid = low + ((high - low) >> 1);
    iter = strs[mid];

    result = cmp(iter, str);

    if (result == 0) {
      return iter;
    } else if (result < 0) {
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }

  return NULL;
}

ret_t object_to_json(tk_object_t* obj, str_t* str) {
  return tk_object_to_json(obj, str, 2, 0, FALSE);
}

static ret_t object_from_json_copy_props(tk_object_t* obj, tk_object_t* src);

static ret_t object_from_json_set_prop(tk_object_t* obj, const char* name, const value_t* v) {
  ret_t ret = RET_OK;
  value_t value;

  if (v->type == VALUE_TYPE_OBJECT) {
    value_t tmp;
    tk_object_t* o = NULL;
    tk_object_t* v_obj = value_object(v);
    if (v_obj->vt->is_collection) {
      o = object_array_create();
    } else {
      o = object_hash_create();
    }
    object_from_json_copy_props(o, v_obj);
    value_set_object(&tmp, o);
    value_deep_copy(&value, &tmp);
    TK_OBJECT_UNREF(o);
  } else {
    value_copy(&value, v);
  }

  if (tk_object_is_instance_of(obj, OBJECT_ARRAY_TYPE)) {
    ret = object_array_push(obj, &value);
  } else {
    ret = tk_object_set_prop(obj, name, &value);
  }

  value_reset(&value);

  return ret;
}

static ret_t object_from_json_copy_props_on_visit(void* ctx, const void* data) {
  named_value_t* nv = (named_value_t*)data;
  tk_object_t* obj = TK_OBJECT(ctx);
  object_from_json_set_prop(obj, nv->name, &(nv->value));
  return RET_OK;
}

inline static ret_t object_from_json_copy_props(tk_object_t* obj, tk_object_t* src) {
  return_value_if_fail(obj != NULL && src != NULL, RET_BAD_PARAMS);

  return tk_object_foreach_prop(src, object_from_json_copy_props_on_visit, obj);
}

ret_t object_from_json(tk_object_t* obj, const char* str) {
  ret_t ret = RET_OK;
  char url[MAX_PATH + 1];
  tk_object_t* obj_json = NULL;
  return_value_if_fail(obj != NULL && str != NULL, RET_BAD_PARAMS);

  data_reader_mem_build_url(str, tk_strlen(str), url);
  obj_json = conf_json_load_ex(url, FALSE, TRUE);
  return_value_if_fail(obj_json != NULL, RET_OOM);

  tk_object_clear_props(obj);
  ret = object_from_json_copy_props(obj, obj_json);

  TK_OBJECT_UNREF(obj_json);

  return ret;
}

bool_t tk_str_indexable(const char* str) {
  bool_t ret = TRUE;
  return_value_if_fail(str != NULL, FALSE);

  ret = (str[0] == '[');
  if (ret) {
    uint32_t len = tk_strlen(str);
    ret = (len >= 3);
    if (ret) {
      ret = (str[len - 1] == ']');
      if (ret) {
        if ((len >= 4) && (str[1] == '-')) {
          ret = tk_isdigit(str[2]);
        } else {
          ret = tk_isdigit(str[1]);
        }
      }
    }
  }

  return ret;
}

double tk_normalize_rad(double value) {
  const double two_pi = 2 * M_PI;
  value = fmod(value, two_pi);
  if (value < 0) {
    value += two_pi;
  }

  return value;
}

bool_t tk_rad_equal(double r1, double r2, double epsilon) {
  r1 = tk_normalize_rad(r1);
  r2 = tk_normalize_rad(r2);

  double diff = fabs(r1 - r2);

  double alt_diff = fabs(2 * M_PI - diff);

  return (diff <= epsilon) || (alt_diff <= epsilon);
}

ret_t tk_str_trim_left(char* str, const char* chars) {
  const char* p = str;
  return_value_if_fail(str != NULL && chars != NULL, RET_BAD_PARAMS);

  while (*p && strchr(chars, *p) != NULL) {
    p++;
  }

  if (p != str) {
    memmove(str, p, strlen(p) + 1);
  }

  return RET_OK;
}

ret_t tk_str_trim_right(char* str, const char* chars) {
  char* p = str + strlen(str) - 1;
  return_value_if_fail(str != NULL && chars != NULL, RET_BAD_PARAMS);
  return_value_if_fail(p >= str, RET_BAD_PARAMS);

  while (p >= str && strchr(chars, *p) != NULL) {
    p--;
  }
  *(p + 1) = '\0';
  
  return RET_OK;
}