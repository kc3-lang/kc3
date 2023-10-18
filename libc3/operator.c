/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include "c3.h"

s_ident * operator_call_ident (const s_ident *op, u8 arity,
                               s_ident *dest)
{
  return env_operator_call_ident(&g_c3_env, op, dest, arity);
}

bool operator_find (const s_ident *op, u8 arity)
{
  return env_operator_find(&g_c3_env, op, arity);
}

bool operator_is_right_associative (const s_ident *op)
{
  return env_operator_is_right_associative(&g_c3_env, op);
}

s8 operator_precedence (const s_ident *op)
{
  return env_operator_precedence(&g_c3_env, op);
}
