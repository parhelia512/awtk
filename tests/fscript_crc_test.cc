#include "tkc/fscript.h"
#include "tkc/object_default.h"
#include "gtest/gtest.h"

TEST(FExr, cksum) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "cksum(\"1\")", &v);
  ASSERT_EQ(value_int(&v), 65486);
  value_reset(&v);
  
  fscript_eval(obj, "cksum(\"123\")", &v);
  ASSERT_EQ(value_int(&v), 65385);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FExr, crc16) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "crc16(\"1\")", &v);
  ASSERT_EQ(value_int(&v), 53362);
  value_reset(&v);
  
  fscript_eval(obj, "crc16(\"123\")", &v);
  ASSERT_EQ(value_int(&v), 40116);
  value_reset(&v);

  OBJECT_UNREF(obj);
}

TEST(FExr, crc32) {
  value_t v;
  object_t* obj = object_default_create();

  fscript_eval(obj, "crc32(\"1\")", &v);
  ASSERT_EQ(value_int(&v), 2212294583);
  value_reset(&v);
  
  fscript_eval(obj, "crc32(\"123\")", &v);
  ASSERT_EQ(value_int(&v), 2286445522);
  value_reset(&v);

  OBJECT_UNREF(obj);
}
