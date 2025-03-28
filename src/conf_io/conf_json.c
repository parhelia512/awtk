﻿/**
 * File:   conf_json.c
 * Author: AWTK Develop Team
 * Brief:  json
 *
 * Copyright (c) 2020 - 2025 Guangzhou ZHIYUAN Electronics Co.,Ltd.
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
 * 2020-05-15 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "conf_io/conf_json.h"
#include "tkc/data_reader_mem.h"
#include "tkc/data_writer_wbuffer.h"
#include "tkc/data_reader_factory.h"
#include "tkc/data_writer_factory.h"

typedef struct _json_parser_t {
  const char* data;
  uint32_t size;
  uint32_t cursor;

  str_t str;
  conf_doc_t* doc;
  conf_node_t* node;
  conf_node_t* current;
} json_parser_t;

static ret_t conf_json_parse_value(json_parser_t* parser);
static ret_t conf_json_skip_all_comments(json_parser_t* parser);
static ret_t conf_json_skip_to_char(json_parser_t* parser, char c);

static ret_t conf_json_parse_name(json_parser_t* parser) {
  str_t* s = &(parser->str);
  conf_node_t* node = NULL;
  uint32_t i = parser->cursor;
  const char* p = parser->data;

  return_value_if_fail(conf_json_skip_to_char(parser, '\"') == RET_OK, RET_BAD_PARAMS);
  i = ++(parser->cursor);
  while (i < parser->size && p[i] != '\"') {
    i++;
  }

  str_set_with_len(s, parser->data + parser->cursor, i - parser->cursor);
  str_unescape(s);
  parser->cursor = i + 1;
  node = conf_doc_create_node(parser->doc, s->str);
  return_value_if_fail(node != NULL, RET_OOM);

  conf_doc_append_child(parser->doc, parser->current, node);
  parser->current = node;

  return RET_OK;
}

static ret_t conf_json_skip_to_char(json_parser_t* parser, char c) {
  while (parser->cursor < parser->size) {
    if (parser->data[parser->cursor] == '\\') {
      parser->cursor += 2;
      continue;
    }
    if (parser->data[parser->cursor] == c) {
      break;
    }
    parser->cursor++;
  }

  return parser->cursor < parser->size ? RET_OK : RET_BAD_PARAMS;
}

static ret_t conf_json_skip_to_value_end(json_parser_t* parser) {
  while (parser->cursor < parser->size) {
    char c = parser->data[parser->cursor];

    if (c == ',' || c == '}' || c == ']') {
      break;
    }
    parser->cursor++;
  }

  return parser->cursor < parser->size ? RET_OK : RET_BAD_PARAMS;
}

static ret_t conf_json_skip_spaces(json_parser_t* parser) {
  while (parser->cursor < parser->size) {
    if (!tk_isspace(parser->data[parser->cursor])) {
      break;
    }
    parser->cursor++;
  }

  return parser->cursor < parser->size ? RET_OK : RET_BAD_PARAMS;
}

static ret_t conf_json_parse_object(json_parser_t* parser) {
  char c = '\0';

  conf_json_skip_all_comments(parser);
  parser->cursor++;
  while (parser->cursor < parser->size) {
    conf_json_skip_all_comments(parser);
    if (parser->data[parser->cursor] == '}') {
      parser->cursor++;
      break;
    }

    return_value_if_fail(conf_json_parse_name(parser) == RET_OK, RET_FAIL);
    return_value_if_fail(conf_json_skip_to_char(parser, ':') == RET_OK, RET_BAD_PARAMS);
    parser->cursor++;
    return_value_if_fail(conf_json_parse_value(parser) == RET_OK, RET_FAIL);
    parser->current = parser->current->parent;

    conf_json_skip_all_comments(parser);
    c = parser->data[parser->cursor];
    if (c == '}') {
      parser->cursor++;
      break;
    } else if (c == ',') {
      parser->cursor++;
    } else {
      log_debug("unexpected char %c at %s\n", c, parser->data + parser->cursor);
      parser->cursor++;
    }
  }

  return RET_OK;
}

static ret_t conf_json_parse_array(json_parser_t* parser) {
  char c = '\0';
  uint32_t i = 0;
  conf_node_t* node = NULL;
  conf_node_t* prev = NULL;
  char name[TK_NAME_LEN + 1];

  parser->cursor++;
  conf_json_skip_all_comments(parser);
  c = parser->data[parser->cursor];
  if (c == ']') {
    parser->cursor++;
    return RET_OK;
  }

  prev = conf_node_get_first_child(parser->current);
  while (parser->cursor < parser->size) {
    tk_snprintf(name, TK_NAME_LEN, "%u", i++);
    node = conf_doc_create_node(parser->doc, name);
    return_value_if_fail(node != NULL, RET_OOM);
    if (prev == NULL) {
      conf_doc_append_child(parser->doc, parser->current, node);
    } else {
      conf_doc_append_sibling(parser->doc, prev, node);
    }
    parser->current = node;
    return_value_if_fail(conf_json_parse_value(parser) == RET_OK, RET_FAIL);
    parser->current = node->parent;
    prev = node;

    conf_json_skip_all_comments(parser);
    c = parser->data[parser->cursor];
    if (c == ']') {
      parser->cursor++;
      break;
    } else if (c == ',') {
      parser->cursor++;
      continue;
    } else {
      log_debug("unexpected char %c at %s\n", c, parser->data + parser->cursor);
    }
  }

  return RET_OK;
}

static ret_t conf_json_parse_bool(json_parser_t* parser) {
  value_t v;
  char c = parser->data[parser->cursor];

  conf_json_skip_to_value_end(parser);
  value_set_bool(&v, (c == 't' || c == 'T'));

  return conf_node_set_value(parser->current, &v);
}

static ret_t conf_json_parse_number(json_parser_t* parser) {
  value_t v;
  str_t* s = &(parser->str);
  uint32_t start = parser->cursor;

  conf_json_skip_to_value_end(parser);
  str_set_with_len(s, parser->data + start, parser->cursor - start);

  if (strchr(s->str, '.') == NULL && strchr(s->str, 'E') == NULL) {
    int64_t n = tk_atol(s->str);
    if (n < INT_MAX && n > INT_MIN) {
      value_set_int32(&v, n);
    } else {
      value_set_int64(&v, n);
    }
  } else {
    value_set_double(&v, tk_atof(s->str));
  }

  return conf_node_set_value(parser->current, &v);
}

static ret_t conf_json_parse_null_string(json_parser_t* parser) {
  value_t v;
  conf_json_skip_to_value_end(parser);
  value_set_str(&v, NULL);

  return conf_node_set_value(parser->current, &v);
}

static ret_t conf_json_parse_string(json_parser_t* parser) {
  value_t v;
  uint32_t start = 0;
  str_t* s = &(parser->str);

  return_value_if_fail(conf_json_skip_to_char(parser, '\"') == RET_OK, RET_BAD_PARAMS);
  start = ++(parser->cursor);
  return_value_if_fail(conf_json_skip_to_char(parser, '\"') == RET_OK, RET_BAD_PARAMS);

  str_set_with_len(s, parser->data + start, parser->cursor - start);
  conf_json_skip_to_value_end(parser);

  str_unescape(s);
  value_set_str(&v, s->str);

  return conf_node_set_value(parser->current, &v);
}

static ret_t conf_json_skip_comment(json_parser_t* parser) {
  char c = 0;
  char next_c = 0;
  bool_t is_block_comment = FALSE;
  bool_t is_line_comment = FALSE;
  const char* p = parser->data;

  conf_json_skip_spaces(parser);
  if ((parser->cursor + 1) < parser->size) {
    c = p[parser->cursor];
    next_c = p[parser->cursor + 1];
    if (c == '/' && next_c == '*') {
      is_block_comment = TRUE;
    } else if (c == '/' && next_c == '/') {
      is_line_comment = TRUE;
    } else {
      return RET_FAIL;
    }

    parser->cursor += 2;
    for (; (parser->cursor + 1) < parser->size; parser->cursor++) {
      c = p[parser->cursor];
      next_c = p[parser->cursor + 1];

      if (is_block_comment) {
        if (c == '*' && next_c == '/') {
          parser->cursor += 2;
          break;
        }
      } else if (is_line_comment) {
        if (c == '\r') {
          parser->cursor++;
          if (next_c == '\n') {
            parser->cursor++;
          }
          break;
        } else if (c == '\n') {
          parser->cursor++;
          break;
        }
      }
    }

    return RET_OK;
  } else {
    return RET_FAIL;
  }
}

static ret_t conf_json_skip_all_comments(json_parser_t* parser) {
  while (conf_json_skip_comment(parser) == RET_OK) {
    if (parser->cursor == parser->size) {
      break;
    }
  }

  return RET_OK;
}

static ret_t conf_json_parse_value(json_parser_t* parser) {
  char c = 0;
  conf_node_t* node = NULL;
  const char* p = parser->data;
  conf_doc_t* doc = parser->doc;

  for (; parser->cursor < parser->size; parser->cursor++) {
    conf_json_skip_all_comments(parser);
    c = p[parser->cursor];

    if (c == '\0' || parser->cursor == parser->size) {
      break;
    } else if (c == '{' || c == '[') {
      if (doc->root == NULL) {
        node = conf_doc_create_node(doc, CONF_NODE_ROOT_NAME);
        return_value_if_fail(node != NULL, RET_OOM);
        doc->root = node;
        parser->current = node;
      }

      parser->current->value_type = CONF_NODE_VALUE_NODE;

      if (c == '[') {
        parser->current->node_type = CONF_NODE_ARRAY;
        return conf_json_parse_array(parser);
      } else {
        return conf_json_parse_object(parser);
      }
    } else if (!tk_isspace(c)) {
      if (c == '\"') {
        return conf_json_parse_string(parser);
      } else if (c == 't' || c == 'f') {
        return conf_json_parse_bool(parser);
      } else if (c == 'n' && strncmp(p + parser->cursor, "null", 4) == 0) {
        return conf_json_parse_null_string(parser);
      } else {
        return conf_json_parse_number(parser);
      }
    }
  }

  return RET_OK;
}

conf_doc_t* conf_doc_load_json(const char* data, int32_t size) {
  json_parser_t parser;
  return_value_if_fail(data != NULL, NULL);
  size = size < 0 ? strlen(data) : size;

  memset(&parser, 0x00, sizeof(parser));

  parser.doc = conf_doc_create(((size) / sizeof(conf_node_t)) + 5);
  return_value_if_fail(parser.doc != NULL, NULL);

  if (str_init(&(parser.str), 32) == NULL) {
    conf_doc_destroy(parser.doc);
    return NULL;
  }

  parser.data = data;
  parser.size = size;

  conf_json_parse_value(&parser);
  str_reset(&(parser.str));

  return parser.doc;
}

static ret_t conf_json_save_node(conf_node_t* node, str_t* str, uint32_t level, uint32_t indent);
static ret_t conf_json_save_node_value(conf_node_t* node, str_t* str, uint32_t level,
                                       uint32_t indent);
static ret_t conf_json_save_node_children(conf_node_t* node, str_t* str, uint32_t level,
                                          uint32_t indent);

static ret_t conf_json_save_node_value_object(conf_node_t* node, str_t* str, uint32_t level,
                                              uint32_t indent) {
  return conf_json_save_node_children(node, str, level, indent);
}

static ret_t conf_json_save_node_value_array(conf_node_t* node, str_t* str, uint32_t level,
                                             uint32_t indent) {
  return conf_json_save_node_children(node, str, level, indent);
}

static ret_t conf_json_save_node_value_simple(conf_node_t* node, str_t* str, uint32_t level) {
  value_t v;
  return_value_if_fail(conf_node_get_value(node, &v) == RET_OK, RET_BAD_PARAMS);

  if (v.type == VALUE_TYPE_BOOL) {
    if (value_bool(&v)) {
      return str_append(str, "true");
    } else {
      return str_append(str, "false");
    }
  } else if (v.type == VALUE_TYPE_STRING) {
    return str_append_json_str(str, value_str(&v));
  } else if (v.type == VALUE_TYPE_WSTRING) {
    str_t s;
    ret_t ret = RET_OK;

    str_init(&s, 0);
    str_from_wstr(&s, value_wstr(&v));
    ret = str_append_json_str(str, s.str);
    str_reset(&s);
    return ret;
  } else {
    double d = value_double(&v);
    if (v.type == VALUE_TYPE_INT64) {
      return str_append_int64(str, d);
    } else if (v.type == VALUE_TYPE_UINT64) {
      return str_append_uint64(str, d);
    } else if (v.type == VALUE_TYPE_UINT32) {
      return str_append_uint32(str, d);
    } else if (d == (int64_t)d) {
      return str_append_int(str, (int)d);
    } else {
      return str_append_double(str, "%.4f", value_double(&v));
    }
  }
}

static ret_t conf_json_save_node_value(conf_node_t* node, str_t* str, uint32_t level,
                                       uint32_t indent) {
  return_value_if_fail(node != NULL, RET_BAD_PARAMS);
  if (node->node_type == CONF_NODE_OBJECT) {
    return conf_json_save_node_value_object(node, str, level, indent);
  } else if (node->node_type == CONF_NODE_ARRAY) {
    return conf_json_save_node_value_array(node, str, level, indent);
  } else {
    return conf_json_save_node_value_simple(node, str, level);
  }
}

static ret_t conf_json_write_indent(str_t* str, int32_t level, uint32_t indent) {
  if (level < 0 || indent == 0) {
    return RET_OK;
  }

  return str_append_n_chars(str, ' ', level * indent);
}

static ret_t conf_json_save_node(conf_node_t* node, str_t* str, uint32_t level, uint32_t indent) {
  const char* name = conf_node_get_name(node);

  if (indent > 0) {
    return_value_if_fail(conf_json_write_indent(str, level, indent) == RET_OK, RET_OOM);
    return_value_if_fail(str_append_json_str(str, name) == RET_OK, RET_OOM);
    return_value_if_fail(str_append(str, " : ") == RET_OK, RET_OOM);
  } else {
    return_value_if_fail(str_append_json_str(str, name) == RET_OK, RET_OOM);
    return_value_if_fail(str_append(str, ":") == RET_OK, RET_OOM);
  }

  return conf_json_save_node_value(node, str, level, indent);
}

static ret_t conf_json_save_node_children_object(conf_node_t* node, str_t* str, uint32_t level,
                                                 uint32_t indent) {
  conf_node_t* iter = conf_node_get_first_child(node);
  return_value_if_fail(node != NULL, RET_BAD_PARAMS);

  if (indent > 0) {
    return_value_if_fail(str_append(str, "{\n") == RET_OK, RET_OOM);
  } else {
    return_value_if_fail(str_append(str, "{") == RET_OK, RET_OOM);
  }

  while (iter != NULL) {
    return_value_if_fail(conf_json_save_node(iter, str, level + 1, indent) == RET_OK, RET_OOM);
    iter = iter->next;
    if (iter != NULL) {
      if (indent > 0) {
        return_value_if_fail(str_append(str, ",\n") == RET_OK, RET_OOM);
      } else {
        return_value_if_fail(str_append(str, ",") == RET_OK, RET_OOM);
      }
    }
  }
  if (indent > 0) {
    return_value_if_fail(str_append(str, "\n") == RET_OK, RET_OOM);
    return_value_if_fail(conf_json_write_indent(str, level, indent) == RET_OK, RET_OOM);
  }
  return_value_if_fail(str_append(str, "}") == RET_OK, RET_OOM);

  return RET_OK;
}

static ret_t conf_json_save_node_children_array(conf_node_t* node, str_t* str, uint32_t level,
                                                uint32_t indent) {
  conf_node_t* iter = conf_node_get_first_child(node);
  return_value_if_fail(node != NULL, RET_BAD_PARAMS);

  if (indent > 0) {
    return_value_if_fail(str_append(str, "[\n") == RET_OK, RET_OOM);
  } else {
    return_value_if_fail(str_append(str, "[") == RET_OK, RET_OOM);
  }

  while (iter != NULL) {
    return_value_if_fail(conf_json_write_indent(str, level + 1, indent) == RET_OK, RET_OOM);
    return_value_if_fail(conf_json_save_node_value(iter, str, level + 1, indent) == RET_OK,
                         RET_OOM);
    iter = iter->next;
    if (iter != NULL) {
      if (indent > 0) {
        return_value_if_fail(str_append(str, ",\n") == RET_OK, RET_OOM);
      } else {
        return_value_if_fail(str_append(str, ",") == RET_OK, RET_OOM);
      }
    }
  }

  if (indent > 0) {
    return_value_if_fail(str_append(str, "\n") == RET_OK, RET_OOM);
    return_value_if_fail(conf_json_write_indent(str, level, indent) == RET_OK, RET_OOM);
  }
  return_value_if_fail(str_append(str, "]") == RET_OK, RET_OOM);

  return RET_OK;
}

static ret_t conf_json_save_node_children(conf_node_t* node, str_t* str, uint32_t level,
                                          uint32_t indent) {
  return_value_if_fail(node != NULL, RET_BAD_PARAMS);

  if (node->node_type == CONF_NODE_OBJECT) {
    return conf_json_save_node_children_object(node, str, level, indent);
  } else if (node->node_type == CONF_NODE_ARRAY) {
    return conf_json_save_node_children_array(node, str, level, indent);
  } else {
    assert(!"invalid type");
    return RET_FAIL;
  }
}

ret_t conf_doc_save_json(conf_doc_t* doc, str_t* str) {
  return_value_if_fail(doc != NULL && doc->root != NULL && str != NULL, RET_BAD_PARAMS);

  str_set(str, "");
  return conf_json_save_node_children(doc->root, str, 0, 4);
}

ret_t conf_doc_save_json_ex(conf_doc_t* doc, str_t* str, uint32_t indent) {
  return_value_if_fail(doc != NULL && doc->root != NULL && str != NULL, RET_BAD_PARAMS);

  str_set(str, "");
  return conf_json_save_node_children(doc->root, str, 0, indent);
}

static conf_doc_t* conf_doc_load_json_reader(data_reader_t* reader) {
  char* data = NULL;
  int32_t rsize = 0;
  conf_doc_t* doc = NULL;
  uint64_t size = data_reader_get_size(reader);
  return_value_if_fail(reader != NULL && size > 0, NULL);
  data = TKMEM_ALLOC(size + 1);
  return_value_if_fail(data != NULL, NULL);

  memset(data, 0x00, size + 1);
  rsize = data_reader_read(reader, 0, data, size);
  if (rsize > 0) {
    doc = conf_doc_load_json(data, rsize);
  }
  TKMEM_FREE(data);

  return doc;
}

static ret_t conf_doc_save_json_writer(conf_doc_t* doc, data_writer_t* writer) {
  str_t str;
  return_value_if_fail(writer != NULL, RET_BAD_PARAMS);

  goto_error_if_fail(str_init(&str, 1024) != NULL);
  goto_error_if_fail(conf_doc_save_json(doc, &str) == RET_OK);
  goto_error_if_fail(data_writer_write(writer, 0, str.str, str.size) == str.size);
  str_reset(&str);

  return RET_OK;

error:
  str_reset(&str);

  return RET_FAIL;
}

tk_object_t* conf_json_load(const char* url, bool_t create_if_not_exist) {
  return conf_obj_create(conf_doc_save_json_writer, conf_doc_load_json_reader, url,
                         create_if_not_exist);
}

tk_object_t* conf_json_load_ex(const char* url, bool_t create_if_not_exist,
                               bool_t use_extend_type) {
  return conf_obj_create_ex(conf_doc_save_json_writer, conf_doc_load_json_reader, url,
                            create_if_not_exist, TRUE);
}

ret_t conf_json_save_as(tk_object_t* obj, const char* url) {
  data_writer_t* writer = NULL;
  conf_doc_t* doc = conf_obj_get_doc(obj);
  return_value_if_fail(doc != NULL && url != NULL, RET_BAD_PARAMS);
  writer = data_writer_factory_create_writer(data_writer_factory(), url);
  return_value_if_fail(writer != NULL, RET_BAD_PARAMS);

  conf_doc_save_json_writer(doc, writer);
  data_writer_destroy(writer);

  return RET_OK;
}

tk_object_t* conf_json_create(void) {
  return conf_json_load(NULL, TRUE);
}

tk_object_t* conf_json_load_from_buff(const void* buff, uint32_t size, bool_t create_if_not_exist) {
  char url[MAX_PATH + 1] = {0};
  return_value_if_fail(buff != NULL, NULL);
  data_reader_mem_build_url(buff, size, url);

  return conf_json_load(url, create_if_not_exist);
}

ret_t conf_json_save_to_buff(tk_object_t* obj, wbuffer_t* wb) {
  char url[MAX_PATH + 1] = {0};
  return_value_if_fail(obj != NULL && wb != NULL, RET_BAD_PARAMS);

  wbuffer_init_extendable(wb);
  data_writer_wbuffer_build_url(wb, url);

  return conf_json_save_as(obj, url);
}
