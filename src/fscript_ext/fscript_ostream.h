﻿/**
 * File:   fscript_ostream.h
 * Author: AWTK Develop Team
 * Brief:  ostream functions for fscript
 *
 * Copyright (c) 2020 - 2025 Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 */

/**
 * History:
 * ================================================================
 * 2021-01-06 Li XianJing <lixianjing@zlg.cn> created
 *
 */

#ifndef TK_FSCRIPT_OSTREAM_H
#define TK_FSCRIPT_OSTREAM_H

#include "tkc/str.h"
#include "tkc/object.h"

BEGIN_C_DECLS

/**
 * @method fscript_ostream_register
 * 注册ostream相关函数。
 *
 * @return {ret_t} 返回RET_OK表示成功，否则表示失败。
 */
ret_t fscript_ostream_register(void);

END_C_DECLS

#endif /*TK_FSCRIPT_OSTREAM_H*/
