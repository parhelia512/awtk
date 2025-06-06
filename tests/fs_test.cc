﻿#include "tkc/fs.h"
#include "tkc/mem.h"
#include "tkc/utils.h"
#include "tkc/darray.h"
#include "gtest/gtest.h"

TEST(Fs, basic) {
  ASSERT_EQ(fs_test(os_fs()), RET_OK);
}

TEST(Fs, read_part) {
  char buff[128];
  uint32_t size = 0;
  const char* str = "hello world";
  const char* filename = "test.bin";

  file_write(filename, str, strlen(str));
  char* ret = (char*)file_read(filename, &size);
  ASSERT_EQ(file_read_part(filename, buff, sizeof(buff), 0), (int32_t)strlen(str));
  ASSERT_EQ(strcmp(ret, str), 0);
  ASSERT_EQ(size, (uint32_t)strlen(str));
  file_remove(filename);
  TKMEM_FREE(ret);
}

TEST(Fs, eof) {
  char buff[128];
  const char* str = "hello world";
  const char* filename = "test.bin";

  file_write(filename, str, strlen(str));

  fs_file_t* f = fs_open_file(os_fs(), filename, "r");
  memset(buff, 0x00, sizeof(buff));
  fs_file_read(f, buff, sizeof(buff));
  ASSERT_STREQ(buff, str);
  ASSERT_EQ(fs_file_eof(f), TRUE);
  fs_file_close(f);

  file_remove(filename);
}

TEST(Fs, read_line1) {
  char buff[128];
  const char* str = "1\r22\r\n333\n4444";
  const char* filename = "test.bin";

  file_write(filename, str, strlen(str));

  fs_file_t* f = fs_open_file(os_fs(), filename, "rb");

  ASSERT_EQ(fs_file_read_line(f, buff, sizeof(buff) - 1), 1);
  ASSERT_STREQ(buff, "1");
  ASSERT_EQ(fs_file_eof(f), FALSE);

  ASSERT_EQ(fs_file_read_line(f, buff, sizeof(buff) - 1), 2);
  ASSERT_STREQ(buff, "22");
  ASSERT_EQ(fs_file_eof(f), FALSE);

  ASSERT_EQ(fs_file_read_line(f, buff, sizeof(buff) - 1), 3);
  ASSERT_STREQ(buff, "333");
  ASSERT_EQ(fs_file_eof(f), FALSE);

  ASSERT_EQ(fs_file_read_line(f, buff, 3), 3);
  ASSERT_STREQ(buff, "444");
  ASSERT_EQ(fs_file_eof(f), FALSE);

  ASSERT_EQ(fs_file_read_line(f, buff, 3), 1);
  ASSERT_STREQ(buff, "4");

  ASSERT_EQ(fs_file_read_line(f, buff, 3), 0);
  ASSERT_EQ(fs_file_eof(f), TRUE);

  fs_file_close(f);

  file_remove(filename);
}

TEST(Fs, read_line2) {
  char buff[256];
  const char* str =
      "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
      "11111111111111111111111111111111111";
  const char* filename = "test.bin";

  file_write(filename, str, strlen(str));

  fs_file_t* f = fs_open_file(os_fs(), filename, "rb");

  ASSERT_EQ(fs_file_read_line(f, buff, sizeof(buff) - 1), 127);
  ASSERT_EQ(fs_file_eof(f), FALSE);

  ASSERT_EQ(fs_file_read_line(f, buff, 3), 0);
  ASSERT_EQ(fs_file_eof(f), TRUE);

  fs_file_close(f);

  file_remove(filename);
}

TEST(Fs, read_line3) {
  char buff[256];
  const char* str =
      "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
      "111111111111111111111111111111111111\r\n";
  const char* filename = "test.bin";

  file_write(filename, str, strlen(str));

  fs_file_t* f = fs_open_file(os_fs(), filename, "rb");

  ASSERT_EQ(fs_file_read_line(f, buff, sizeof(buff) - 1), 128);
  ASSERT_EQ(fs_file_eof(f), FALSE);

  ASSERT_EQ(fs_file_read_line(f, buff, 3), 0);
  ASSERT_EQ(fs_file_eof(f), TRUE);

  fs_file_close(f);

  file_remove(filename);
}

TEST(Fs, read_line4) {
  char buff[256];
  const char* str =
      "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
      "11111111111111111111111111111111111\r\nabc";
  const char* filename = "test.bin";

  file_write(filename, str, strlen(str));

  fs_file_t* f = fs_open_file(os_fs(), filename, "rb");

  ASSERT_EQ(fs_file_read_line(f, buff, sizeof(buff) - 1), 127);
  ASSERT_EQ(fs_file_read_line(f, buff, sizeof(buff) - 1), 3);
  ASSERT_EQ(fs_file_eof(f), FALSE);

  ASSERT_EQ(fs_file_read_line(f, buff, 3), 0);
  ASSERT_EQ(fs_file_eof(f), TRUE);

  fs_file_close(f);

  file_remove(filename);
}

TEST(Fs, create_dir_r) {
  char cwd[MAX_PATH + 1];
  ASSERT_EQ(fs_get_cwd(os_fs(), cwd), RET_OK);

  ASSERT_EQ(fs_create_dir_r(os_fs(), "a/b/c/d"), RET_OK);
  ASSERT_EQ(fs_dir_exist(os_fs(), "a/b/c/d"), TRUE);
  ASSERT_EQ(file_write("a/b/c/d/test.txt", "hello", 5), RET_OK);
  ASSERT_EQ(fs_change_dir(os_fs(), "a/b/c/d"), RET_OK);
  ASSERT_EQ(fs_change_dir(os_fs(), cwd), RET_OK);

  ASSERT_EQ(fs_remove_dir_r(os_fs(), "a"), RET_OK);
  ASSERT_EQ(fs_dir_exist(os_fs(), "a"), FALSE);
}

TEST(Fs, dir_empty) {
  ASSERT_EQ(fs_create_dir_r(os_fs(), "a/b/c/d"), RET_OK);
  ASSERT_EQ(fs_dir_exist(os_fs(), "a/b/c/d"), TRUE);

  ASSERT_EQ(fs_dir_is_empty(os_fs(), "a/b/c/d"), TRUE);
  ASSERT_EQ(file_write("a/b/c/d/test.txt", "hello", 5), RET_OK);
  ASSERT_EQ(fs_dir_is_empty(os_fs(), "a/b/c/d"), FALSE);

  ASSERT_EQ(fs_remove_dir_r(os_fs(), "a"), RET_OK);
  ASSERT_EQ(fs_dir_exist(os_fs(), "a"), FALSE);
}

TEST(Fs, copy_file) {
  const char* src = "./test.txt";
  const char* dst = "./a/b/test.txt";

  ASSERT_EQ(file_write(src, "hello", 5), RET_OK);
  ASSERT_EQ(file_exist(src), TRUE);
  ASSERT_EQ(file_exist(dst), FALSE);

  ASSERT_EQ(fs_copy_file(os_fs(), src, dst), RET_OK);
  ASSERT_EQ(fs_remove_dir_r(os_fs(), "a"), RET_OK);

  dst = "test1.txt";
  ASSERT_EQ(fs_copy_file(os_fs(), src, dst), RET_OK);
  ASSERT_EQ(file_remove(src), RET_OK);
  ASSERT_EQ(file_remove(dst), RET_OK);

  ASSERT_EQ(file_exist(src), FALSE);
  ASSERT_EQ(file_exist(dst), FALSE);
}

TEST(Fs, copy_dir) {
  const char* src = "./a";
  const char* dst = "./b";

  ASSERT_EQ(fs_create_dir_r(os_fs(), "./a/a1"), RET_OK);
  ASSERT_EQ(fs_create_dir_r(os_fs(), "./a/a2"), RET_OK);
  ASSERT_EQ(fs_create_dir_r(os_fs(), "./a/a3/a4"), RET_OK);
  ASSERT_EQ(file_write("./a/a3/a4/test.txt", "hello", 5), RET_OK);

  ASSERT_EQ(fs_copy_dir(os_fs(), src, dst), RET_OK);
  ASSERT_EQ(dir_exist("./b"), TRUE);
  ASSERT_EQ(dir_exist("./b/a1"), TRUE);
  ASSERT_EQ(dir_exist("./b/a2"), TRUE);
  ASSERT_EQ(dir_exist("./b/a3"), TRUE);
  ASSERT_EQ(file_exist("./b/a3/a4/test.txt"), TRUE);

  ASSERT_EQ(fs_copy_dir_ex(os_fs(), src, dst, FALSE), RET_OK);
  ASSERT_EQ(dir_exist("./b"), TRUE);
  ASSERT_EQ(dir_exist("./b/a1"), TRUE);
  ASSERT_EQ(dir_exist("./b/a2"), TRUE);
  ASSERT_EQ(dir_exist("./b/a3"), TRUE);
  ASSERT_EQ(file_exist("./b/a3/a4/test.txt"), TRUE);

  ASSERT_EQ(fs_remove_dir_r(os_fs(), "a"), RET_OK);
  ASSERT_EQ(fs_remove_dir_r(os_fs(), "b"), RET_OK);
}

static ret_t on_file(void* ctx, const void* data) {
  const char* filename = (const char*)data;
  const char* extname = (const char*)ctx;

  if (tk_str_end_with(filename, extname)) {
    log_debug("%s\n", filename);
  }
  return RET_OK;
}

TEST(Fs, foreach_file) {
  fs_foreach_file("tests/testdata", on_file, (void*)".json");
}

static int on_dir_cmp(const void* a, const void* ctx) {
  return strstr((const char*)a, (const char*)ctx) ? 0 : -1;
}

static ret_t on_dir(void* ctx, const void* data) {
  darray_t* dirs = (darray_t*)ctx;
  const char* dpath = (const char*)data;

  darray_push(dirs, tk_strdup(dpath));
  return RET_OK;
}

TEST(Fs, foreach_dir) {
  darray_t* dirs = darray_create(4, default_destroy, (tk_compare_t)on_dir_cmp);

  ASSERT_EQ(fs_create_dir_r(os_fs(), "./tmp1/a1/b1"), RET_OK);
  ASSERT_EQ(fs_create_dir_r(os_fs(), "./tmp1/a1/b2"), RET_OK);
  ASSERT_EQ(fs_create_dir_r(os_fs(), "./tmp1/a1/b3"), RET_OK);

  ASSERT_EQ(file_write("./tmp1/s.txt", "hello", 5), RET_OK);
  ASSERT_EQ(file_write("./tmp1/a1/s.txt", "hello", 5), RET_OK);

  ASSERT_EQ(fs_create_dir_r(os_fs(), "./tmp1/a2/b1"), RET_OK);
  ASSERT_EQ(fs_create_dir_r(os_fs(), "./tmp1/a2/b2"), RET_OK);
  ASSERT_EQ(fs_create_dir_r(os_fs(), "./tmp1/a2/b2/c1"), RET_OK);
  ASSERT_EQ(fs_create_dir_r(os_fs(), "./tmp1/a2/b2/c2"), RET_OK);
  ASSERT_EQ(file_write("./tmp1/a2/s.txt", "hello", 5), RET_OK);
  ASSERT_EQ(file_write("./tmp1/a2/b2/c1/s.txt", "hello", 5), RET_OK);

  ASSERT_EQ(fs_create_dir_r(os_fs(), "./tmp1/a3"), RET_OK);
  ASSERT_EQ(file_write("./tmp1/a3/s.txt", "hello", 5), RET_OK);

  fs_foreach_dir("./tmp1", 1, on_dir, dirs);
  ASSERT_EQ(dirs->size, 3u);
  ASSERT_TRUE(darray_find(dirs, (void*)"a1") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"a2") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"a3") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"b1") == NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"s.txt") == NULL);
  darray_clear(dirs);

  fs_foreach_dir("./tmp1", 2, on_dir, dirs);
  ASSERT_EQ(dirs->size, 3u + 5u);
  ASSERT_TRUE(darray_find(dirs, (void*)"b1") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"b2") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"b3") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"c1") == NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"s.txt") == NULL);
  darray_clear(dirs);

  fs_foreach_dir("./tmp1", 0xffff, on_dir, dirs);
  ASSERT_EQ(dirs->size, 3u + 5u + 2u);
  ASSERT_TRUE(darray_find(dirs, (void*)"b1") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"b2") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"b3") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"c1") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"c2") != NULL);
  ASSERT_TRUE(darray_find(dirs, (void*)"s.txt") == NULL);
  darray_clear(dirs);

  darray_destroy(dirs);

  ASSERT_EQ(fs_remove_dir_r(os_fs(), "./tmp1"), RET_OK);
}

#ifdef WIN32
TEST(Fs, file_stat) {
  fs_stat_info_t fst;
  const char* str =
      "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
      "11111111111111111111111111111111111\r\nabc";
  const char* filename = "test.bin";

  ASSERT_EQ(file_write(filename, str, strlen(str)), RET_OK);

  fs_file_t* fp = fs_open_file(os_fs(), filename, "rb");
  ASSERT_EQ(fp != NULL, true);
  ASSERT_EQ(fs_file_stat(fp, &fst), RET_OK);
  ASSERT_EQ(fs_file_close(fp), RET_OK);
  ASSERT_EQ(file_remove(filename), RET_OK);
}

TEST(Fs, stat) {
  fs_stat_info_t st;
  ASSERT_EQ(fs_stat(os_fs(), "c:", &st), RET_OK);
  ASSERT_EQ(fs_stat(os_fs(), "c:\\", &st), RET_OK);
  ASSERT_EQ(fs_stat(os_fs(), "c://", &st), RET_OK);

  ASSERT_EQ(fs_create_dir_r(os_fs(), "c:\\awtk_test\\test"), RET_OK);
  ASSERT_EQ(dir_exist("c:\\awtk_test\\test"), TRUE);
  ASSERT_EQ(fs_remove_dir_r(os_fs(), "c:\\awtk_test"), RET_OK);
  ASSERT_EQ(dir_exist("c:\\awtk_test"), FALSE);

  ASSERT_EQ(fs_create_dir_r(os_fs(), "c:\\致远电子-awtk\\test"), RET_OK);
  ASSERT_EQ(dir_exist("c:\\致远电子-awtk\\test"), TRUE);
  ASSERT_EQ(fs_remove_dir_r(os_fs(), "c:\\致远电子-awtk"), RET_OK);
  ASSERT_EQ(dir_exist("c:\\致远电子-awtk"), FALSE);

  ASSERT_EQ(fs_create_dir_r(os_fs(), "c://awtk_test//test"), RET_OK);
  ASSERT_EQ(dir_exist("c:/awtk_test/test"), TRUE);
  ASSERT_EQ(fs_remove_dir_r(os_fs(), "c:/awtk_test"), RET_OK);
  ASSERT_EQ(dir_exist("c:/awtk_test"), FALSE);
}
#endif /*WIN32*/
