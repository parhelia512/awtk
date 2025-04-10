/**
 * File:   awtk_main.c
 * Author: AWTK Develop Team
 * Brief:  awtk main
 *
 * Copyright (c) 2018 - 2020  Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2018-02-16 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "awtk.h"

BEGIN_C_DECLS
extern ret_t assets_init(void);
END_C_DECLS

#ifndef APP_DEFAULT_FONT
#define APP_DEFAULT_FONT "default"
#endif /*APP_DEFAULT_FONT*/

#ifndef LCD_WIDTH
#define LCD_WIDTH 320
#endif /*LCD_WIDTH*/

#ifndef LCD_HEIGHT
#define LCD_HEIGHT 480
#endif /*LCD_HEIGHT*/

#ifndef APP_TYPE
#define APP_TYPE APP_SIMULATOR
#endif /*APP_TYPE*/

#ifndef GLOBAL_INIT
#define GLOBAL_INIT()
#endif /*GLOBAL_INIT*/

#ifndef GLOBAL_EXIT
#define GLOBAL_EXIT()
#endif /*GLOBAL_EXIT*/

#ifndef FINAL_EXIT
#define FINAL_EXIT()
#endif /*FINAL_EXIT*/

#ifndef APP_NAME
#define APP_NAME "awtk"
#endif /*APP_NAME*/

#ifndef APP_RES_ROOT
#define APP_RES_ROOT NULL
#endif /*APP_RES_ROOT*/

#ifndef APP_ENABLE_CONSOLE
#define APP_ENABLE_CONSOLE TRUE
#endif /*APP_ENABLE_CONSOLE*/

#ifndef APP_USE_STD_FONT_SIZE
#define APP_USE_STD_FONT_SIZE FALSE
#endif /*APP_USE_STD_FONT_SIZE*/

#include "base/custom_keys.inc"
#include "base/asset_loader_zip.h"

#ifdef USE_GUI_MAIN
int gui_app_start_ex(int lcd_w, int lcd_h, const char* res_root);

int gui_app_start(int lcd_w, int lcd_h) {
  return gui_app_start_ex(lcd_w, lcd_h, APP_RES_ROOT);
}

int gui_app_start_ex(int lcd_w, int lcd_h, const char* res_root) {
  tk_init(lcd_w, lcd_h, APP_MOBILE, APP_NAME, res_root);
#elif defined(MOBILE_APP) && defined(WITH_SDL)
int SDL_main(int argc, char* argv[]) {
  int32_t lcd_w = 0;
  int32_t lcd_h = 0;
  tk_init(lcd_w, lcd_h, APP_MOBILE, APP_NAME, APP_RES_ROOT);

#elif defined(WIN32) && !defined(MINGW)
#include <windows.h>
#ifndef MAX_ARGV
#define MAX_ARGV 7
#endif

static void command_line_to_argv(char* lpcmdline, char* argv[MAX_ARGV], int32_t* argc) {
  int32_t i = 1;
  char* p = lpcmdline;

  argv[0] = "awtk.exe";

  if (p == NULL || *p == '\0') {
    argv[1] = NULL;
    return;
  }

  for (i = 1; i < MAX_ARGV; i++) {
    argv[i] = p;
    if (*p == '\"') {
      argv[i] = p + 1;
      p = strchr(p + 1, '\"');
      if (p == NULL) break;
      *p++ = '\0';
      if (*p == 0) break;
    } else {
      p = strchr(p, ' ');
    }
    if (p == NULL) {
      break;
    }

    while (*p == ' ') {
      *p++ = '\0';
    }
  }
  *argc = i + 1;

  return;
}

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPWSTR lpcmdline, int ncmdshow) {
  str_t str;
  int argc = 1;
  char* argv[MAX_ARGV];
  bool_t enable_console = APP_ENABLE_CONSOLE;
  int32_t lcd_w = LCD_WIDTH;
  int32_t lcd_h = LCD_HEIGHT;

  str_init(&str, 0);
  str_from_wstr(&str, lpcmdline);
  command_line_to_argv(str.str, argv, &argc);


  tk_pre_init();
#ifdef ON_CMD_LINE
  ON_CMD_LINE(argc, argv);
#else
  if (argc >= 2) {
    lcd_w = tk_atoi(argv[1]);
  }
  if (argc >= 3) {
    lcd_h = tk_atoi(argv[2]);
  }
  if (argc >= 4) {
    enable_console = tk_atob(argv[3]);
  }
#endif /*ON_CMD_LINE*/

  if (enable_console) {
    TK_ENABLE_CONSOLE();
  }

  tk_init(lcd_w, lcd_h, APP_TYPE, APP_NAME, APP_RES_ROOT);
#else
int main(int argc, char* argv[]) {
  bool_t enable_console = APP_ENABLE_CONSOLE;
  int32_t lcd_w = LCD_WIDTH;
  int32_t lcd_h = LCD_HEIGHT;

  tk_pre_init();
#ifdef ON_CMD_LINE
  (void)enable_console;
  ON_CMD_LINE(argc, argv);
#else
  if (argc >= 2) {
    lcd_w = tk_atoi(argv[1]);
  }
  if (argc >= 3) {
    lcd_h = tk_atoi(argv[2]);
  }

#ifdef WINDOWS
  if (argc >= 4) {
    enable_console = tk_atob(argv[3]);
  }
  if (enable_console) {
    TK_ENABLE_CONSOLE();
  }
#else
  (void)enable_console;
#endif

#endif /*ON_CMD_LINE*/
  tk_init(lcd_w, lcd_h, APP_TYPE, APP_NAME, APP_RES_ROOT);
#endif

#ifdef ASSETS_ZIP
  assets_manager_set_res_root(assets_manager(), "");
  log_debug("Load assets from zip: %s\n", ASSETS_ZIP);
  assets_manager_set_loader(assets_manager(), asset_loader_zip_create(ASSETS_ZIP));
#elif defined(ASSETS_CUSTOM_INIT)
  ASSETS_CUSTOM_INIT();
#endif /*ASSETS_ZIP*/

#if defined(WITH_LCD_PORTRAIT)
  if (lcd_w > lcd_h) {
    tk_set_lcd_orientation(LCD_ORIENTATION_90);
  }
#endif /*WITH_LCD_PORTRAIT*/

#ifdef WITH_LCD_LANDSCAPE
  if (lcd_w < lcd_h) {
    tk_set_lcd_orientation(LCD_ORIENTATION_90);
  }
#endif /*WITH_LCD_PORTRAIT*/

#ifndef TK_IS_PC
#ifdef APP_LCD_ORIENTATION
#if defined(APP_ENABLE_FAST_LCD_PORTRAIT)
  tk_enable_fast_lcd_portrait(TRUE);
#endif
  tk_set_lcd_orientation(APP_LCD_ORIENTATION);
#endif
#endif/*TK_IS_PC*/

  font_manager_set_standard_font_size(font_manager(), APP_USE_STD_FONT_SIZE);
  system_info_set_default_font(system_info(), APP_DEFAULT_FONT);
  assets_init();
#ifndef WITH_FS_RES
  locale_info_reload(locale_info());
#endif

#ifndef WITHOUT_EXT_WIDGETS
  tk_ext_widgets_init();
#endif /*WITHOUT_EXT_WIDGETS*/

#ifdef NDEBUG
  log_set_log_level(LOG_LEVEL_INFO);
#else
  log_set_log_level(LOG_LEVEL_DEBUG);
#endif /*NDEBUG*/
  log_info("Build at: %s %s\n", __DATE__, __TIME__);

#ifdef ENABLE_CURSOR
  window_manager_set_cursor(window_manager(), "cursor");
#endif /*ENABLE_CURSOR*/

#ifdef WIN32
  setvbuf(stdout, NULL, _IONBF, 0);
#elif defined(HAS_STDIO)
  setlinebuf(stdout);
#endif /*WIN32*/

  GLOBAL_INIT();
#if defined(APP_DEFAULT_LANGUAGE) && defined(APP_DEFAULT_COUNTRY)
  locale_info_change(locale_info(), APP_DEFAULT_LANGUAGE, APP_DEFAULT_COUNTRY);
#endif /*APP_DEFAULT_LANGUAGE and APP_DEFAULT_LANGUAGE*/
  custom_keys_init(TRUE);
  application_init();
  tk_run();
  application_exit();
  custom_keys_deinit(TRUE);
  
  GLOBAL_EXIT();
  tk_exit();

  FINAL_EXIT();
#ifdef HAS_STDIO
  fflush(stdout);
#endif /*HAS_STDIO*/

#if defined(WIN32) && !defined(MINGW)
  str_reset(&str);
#endif /*WIN32*/

#if defined(IOS) || defined(ANDROID)
  exit(0);
#endif /*IOS | ANDROID*/

  return 0;
}
