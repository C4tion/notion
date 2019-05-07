/*
 * ion/ioncore/key.h
 *
 * Copyright (c) Tuomo Valkonen 1999-2007.
 *
 * See the included file LICENSE for details.
 */

#ifndef ION_IONCORE_KEY_H
#define ION_IONCORE_KEY_H

#include <X11/keysym.h>

#include "common.h"
#include "clientwin.h"

extern void ioncore_do_handle_keypress(XKeyEvent *ev);
extern void clientwin_quote_next(WClientWin *cwin);
extern bool ioncore_current_key(uint *kcb, uint *state, bool *sub);

#endif /* ION_IONCORE_KEY_H */
