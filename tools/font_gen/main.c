/**
 * File:   main.c
 * Author: AWTK Develop Team
 * Brief:  bitmap font generator
 *
 * Copyright (c) 2018 - 2024  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-01-21 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/platform.h"
#include "common/utils.h"
#include "font_gen.h"
#include "font_loader/font_loader_bitmap.h"
#ifdef WITH_STB_FONT
#include "font_loader/font_loader_stb.h"
#else
#include "font_loader/font_loader_ft.h"
#endif /*WITH_STB_FONT*/

ret_t gen_one(const char* ttf_filename, const char* str_filename, const char* out_filename,
              const char* theme_name, const char* res_dir, uint32_t font_size,
              glyph_format_t format, bool_t mono) {
  uint32_t size = 0;
  font_t* font = NULL;
  char* str_buff = NULL;
  uint8_t* ttf_buff = NULL;
  exit_if_need_not_update_for_infiles(out_filename, 2, ttf_filename, str_filename);

  ttf_buff = (uint8_t*)read_file(ttf_filename, &size);
  return_value_if_fail(ttf_buff != NULL, RET_FAIL);
#ifdef WITH_STB_FONT
  if (mono) {
    font = font_stb_mono_create("default", ttf_buff, size);
  } else {
    font = font_stb_create("default", ttf_buff, size);
  }
#else
  if (mono) {
    font = font_ft_mono_create("default", ttf_buff, size);
  } else {
    font = font_ft_create("default", ttf_buff, size);
  }
#endif /*WITH_STB_FONT*/

  str_buff = read_file(str_filename, &size);
  if (str_buff == NULL) {
    log_debug("read %s failed\n", str_filename);
  }
  return_value_if_fail(str_buff != NULL, 0);

  if (font != NULL) {
    font_gen(font, (uint16_t)font_size, format, str_buff, out_filename, theme_name, res_dir);
  }

  TKMEM_FREE(ttf_buff);
  TKMEM_FREE(str_buff);

  return RET_OK;
}

int wmain(int argc, wchar_t* argv[]) {
  bool_t mono = FALSE;
  uint32_t font_size = 20;
  const char* res_dir = NULL;
  const char* theme_name = NULL;
  glyph_format_t format = GLYPH_FMT_ALPHA;

  platform_prepare();

  if (argc < 5) {
    printf(
        "Usage: %S ttf_filename str_filename out_filename font_size [mono|4bits|alpha] theme "
        "res_dir\n",
        argv[0]);

    return 0;
  }

  font_size = tk_watoi(argv[4]);

  if (argc > 5) {
    const wchar_t* format_name = argv[5];
    if (tk_wstr_eq(format_name, L"mono")) {
      mono = TRUE;
      format = GLYPH_FMT_MONO;
    } else if (tk_wstr_eq(format_name, L"4bits")) {
      format = GLYPH_FMT_ALPHA4;
    }
  }

  str_t str_theme = {0};
  str_init(&str_theme, 0);
  if (argc > 6) {
    str_from_wstr(&str_theme, argv[6]);
    theme_name = str_theme.str;
  }

  str_t str_res_dir = {0};
  str_init(&str_res_dir, 0);
  if (argc > 7) {
    str_from_wstr(&str_res_dir, argv[7]);
    res_dir = str_res_dir.str;
  }

  str_t ttf_file;
  str_t str_file;
  str_t out_file;

  str_init(&ttf_file, 0);
  str_init(&str_file, 0);
  str_init(&out_file, 0);

  str_from_wstr(&ttf_file, argv[1]);
  str_from_wstr(&str_file, argv[2]);
  str_from_wstr(&out_file, argv[3]);

  gen_one(ttf_file.str, str_file.str, out_file.str, theme_name, res_dir, font_size, format, mono);

  str_reset(&ttf_file);
  str_reset(&str_file);
  str_reset(&out_file);
  str_reset(&str_theme);
  str_reset(&str_res_dir);

  printf("done\n");

  return 0;
}

#include "common/main.inc"
