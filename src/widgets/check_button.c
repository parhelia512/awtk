﻿/**
 * File:   check_button.c
 * Author: AWTK Develop Team
 * Brief:  check_button
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
 * 2018-02-09 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "widgets/check_button.h"
#include "base/image_manager.h"
#include "base/widget_vtable.h"

static ret_t check_button_set_radio(widget_t* widget, bool_t radio);

static ret_t check_button_on_event(widget_t* widget, event_t* e) {
  ret_t ret = RET_OK;
  uint16_t type = e->type;
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL, RET_BAD_PARAMS);

  switch (type) {
    case EVT_POINTER_DOWN: {
      check_button->pressed = TRUE;
      widget_set_state(widget, WIDGET_STATE_PRESSED);
      widget_grab(widget->parent, widget);
      break;
    }
    case EVT_POINTER_DOWN_ABORT: {
      check_button->pressed = FALSE;
      widget_ungrab(widget->parent, widget);
      widget_set_state(widget, WIDGET_STATE_NORMAL);
      break;
    }
    case EVT_CLICK: {
      if (check_button->radio) {
        check_button_set_value(widget, TRUE);
      } else {
        check_button_set_value(widget, !(check_button->value));
      }
      break;
    }
    case EVT_POINTER_UP: {
      pointer_event_t* evt = (pointer_event_t*)e;
      if (check_button->pressed && widget_is_point_in(widget, evt->x, evt->y, FALSE)) {
        pointer_event_t click;
        ret =
            widget_dispatch(widget, pointer_event_init(&click, EVT_CLICK, widget, evt->x, evt->y));
      }

      check_button->pressed = FALSE;
      widget_ungrab(widget->parent, widget);
      widget_set_state(widget, WIDGET_STATE_NORMAL);
      break;
    }
    case EVT_POINTER_LEAVE:
      widget_set_state(widget, WIDGET_STATE_NORMAL);
      break;
    case EVT_POINTER_ENTER:
      widget_set_state(widget, WIDGET_STATE_OVER);
      break;
    default:
      break;
  }

  return ret;
}

static ret_t check_button_on_paint_self(widget_t* widget, canvas_t* c) {
  return widget_paint_helper(widget, c, NULL, NULL);
}

static ret_t check_button_set_value_only(widget_t* widget, bool_t value) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL, RET_BAD_PARAMS);
  check_button->indeterminate = FALSE;
  if (check_button->value != value) {
    value_change_event_t evt;
    value_change_event_init(&evt, EVT_VALUE_WILL_CHANGE, widget);
    value_set_bool(&(evt.old_value), check_button->value);
    value_set_bool(&(evt.new_value), value);

    if (widget_dispatch(widget, (event_t*)&evt) != RET_STOP) {
      check_button->value = value;
      evt.e.type = EVT_VALUE_CHANGED;
      widget_dispatch(widget, (event_t*)&evt);
      widget_set_need_update_style(widget);
      widget_invalidate_force(widget, NULL);
    }
  }

  return RET_OK;
}

ret_t check_button_set_value(widget_t* widget, bool_t value) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL, RET_BAD_PARAMS);

  if (check_button->radio && widget->parent != NULL && value) {
    widget_t* parent = widget->parent;

    WIDGET_FOR_EACH_CHILD_BEGIN(parent, iter, i)
    if (iter != widget && iter->vt == widget->vt) {
      check_button_set_value_only(iter, !value);
    }
    WIDGET_FOR_EACH_CHILD_END();
  }
  check_button_set_value_only(widget, value);

  return RET_OK;
}

static ret_t check_button_get_prop(widget_t* widget, const char* name, value_t* v) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  ENSURE(check_button);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    value_set_bool(v, check_button->value);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_STATE_FOR_STYLE)) {
    if (!check_button->radio && check_button->indeterminate) {
      const char* state = widget_get_state_for_style(widget, FALSE, FALSE);
      if (tk_str_eq(state, WIDGET_STATE_NORMAL)) {
        state = WIDGET_STATE_NORMAL_OF_INDETERMINATE;
      } else if (tk_str_eq(state, WIDGET_STATE_PRESSED)) {
        state = WIDGET_STATE_PRESSED_OF_INDETERMINATE;
      } else if (tk_str_eq(state, WIDGET_STATE_OVER)) {
        state = WIDGET_STATE_OVER_OF_INDETERMINATE;
      } else if (widget_is_focusable(widget) && widget->focused) {
        state = WIDGET_STATE_FOCUSED_OF_INDETERMINATE;
      } else if (tk_str_eq(state, WIDGET_STATE_DISABLE)) {
        state = WIDGET_STATE_DISABLE_OF_INDETERMINATE;
      }
      value_set_str(v, state);
    } else {
      value_set_str(v, widget_get_state_for_style(widget, FALSE, check_button->value));
    }
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_RADIO)) {
    value_set_bool(v, check_button->radio);
    return RET_OK;
  } else if (tk_str_eq(name, WIDGET_PROP_TYPE)) {
    if (check_button->type != NULL) {
      value_set_str(v, check_button->type);
    } else {
      value_set_str(v, widget->vt->type);
    }
    return RET_OK;
  } else if (tk_str_eq(name, CHECK_BUTTON_PROP_INDETERMINATE)) {
    value_set_bool(v, check_button->indeterminate);
    return RET_OK;
  }

  return RET_NOT_FOUND;
}

static ret_t check_button_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_VALUE)) {
    return check_button_set_value(widget, value_bool(v));
  } else if (tk_str_eq(name, WIDGET_PROP_RADIO)) {
    check_button_set_radio(widget, value_bool(v));
    return RET_OK;
  } else if (tk_str_eq(name, CHECK_BUTTON_PROP_INDETERMINATE)) {
    return check_button_set_indeterminate(widget, value_bool(v));
  }

  return RET_NOT_FOUND;
}

static ret_t radio_button_init(widget_t* widget) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL, RET_BAD_PARAMS);

  check_button->radio = FALSE;
  return RET_OK;
}

static ret_t radio_button_init_radio(widget_t* widget) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL, RET_BAD_PARAMS);

  check_button->radio = TRUE;
  return RET_OK;
}

static const char* s_check_button_properties[] = {WIDGET_PROP_VALUE, NULL};
TK_DECL_VTABLE(check_button) = {
    .inputable = TRUE,
    .size = sizeof(check_button_t),
    .type = WIDGET_TYPE_CHECK_BUTTON,
    .space_key_to_activate = TRUE,
    .return_key_to_activate = TRUE,
    .init = radio_button_init,
    .clone_properties = s_check_button_properties,
    .persistent_properties = s_check_button_properties,
    .get_parent_vt = TK_GET_PARENT_VTABLE(widget),
    .create = check_button_create,
    .on_event = check_button_on_event,
    .on_paint_self = check_button_on_paint_self,
    .get_prop = check_button_get_prop,
    .set_prop = check_button_set_prop,
};

TK_DECL_VTABLE(radio_button) = {
    .inputable = TRUE,
    .size = sizeof(check_button_t),
    .type = WIDGET_TYPE_RADIO_BUTTON,
    .space_key_to_activate = TRUE,
    .return_key_to_activate = TRUE,
    .init = radio_button_init_radio,
    .clone_properties = s_check_button_properties,
    .get_parent_vt = TK_GET_PARENT_VTABLE(widget),
    .create = check_button_create_radio,
    .on_event = check_button_on_event,
    .on_paint_self = check_button_on_paint_self,
    .get_prop = check_button_get_prop,
    .set_prop = check_button_set_prop,
};

widget_t* check_button_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(check_button), x, y, w, h);
  return_value_if_fail(radio_button_init(widget) == RET_OK, NULL);

  check_button_set_value_only(widget, FALSE);

  return widget;
}

widget_t* check_button_create_radio(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  widget_t* widget = widget_create(parent, TK_REF_VTABLE(radio_button), x, y, w, h);
  return_value_if_fail(radio_button_init_radio(widget) == RET_OK, NULL);

  check_button_set_value_only(widget, FALSE);

  return widget;
}

static ret_t check_button_set_radio(widget_t* widget, bool_t radio) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL, RET_BAD_PARAMS);
  check_button->radio = radio;
  if (radio) {
    widget->vt = TK_REF_VTABLE(radio_button);
  } else {
    widget->vt = TK_REF_VTABLE(check_button);
  }
  widget_set_need_update_style(widget);
  widget_invalidate_force(widget, NULL);

  return RET_OK;
}

widget_t* check_button_cast(widget_t* widget) {
  return_value_if_fail(
      WIDGET_IS_INSTANCE_OF(widget, check_button) || WIDGET_IS_INSTANCE_OF(widget, radio_button),
      NULL);

  return widget;
}

widget_t* check_button_get_checked_button(widget_t* widget) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL, NULL);

  if (check_button->radio && widget->parent != NULL) {
    widget_t* parent = widget->parent;

    WIDGET_FOR_EACH_CHILD_BEGIN(parent, iter, i)
    if (iter->vt == widget->vt) {
      check_button_t* b = CHECK_BUTTON(iter);
      if (b->value) {
        return iter;
      }
    }
    WIDGET_FOR_EACH_CHILD_END();
  }

  return NULL;
}

widget_t* check_button_create_ex(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h, const char* type,
                                 bool_t radio) {
  const widget_vtable_t* vt = radio ? TK_REF_VTABLE(radio_button) : TK_REF_VTABLE(check_button);
  widget_t* widget = widget_create(parent, vt, x, y, w, h);
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL, NULL);

  check_button->type = type;

  return widget;
}

ret_t check_button_set_indeterminate(widget_t* widget, bool_t indeterminate) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL && !check_button->radio, RET_BAD_PARAMS);
  check_button->indeterminate = indeterminate;
  return RET_OK;
}

bool_t check_button_get_indeterminate(widget_t* widget) {
  check_button_t* check_button = CHECK_BUTTON(widget);
  return_value_if_fail(check_button != NULL && !check_button->radio, FALSE);
  return check_button->indeterminate;
}
