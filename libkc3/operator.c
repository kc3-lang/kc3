/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "env.h"
#include "operator.h"

s8 operator_arity (const s_ident *op)
{
  return env_operator_arity(&g_kc3_env, op);
}

bool * operator_find (const s_ident *op, bool *dest)
{
  return env_operator_find(&g_kc3_env, op, dest);
}

s_ident * operator_ident (const s_ident *op, s_ident *dest)
{
  return env_operator_ident(&g_kc3_env, op, dest);
}

bool * operator_is_right_associative (const s_ident *op, bool *dest)
{
  return env_operator_is_right_associative(&g_kc3_env, op, dest);
}

s8 operator_precedence (const s_ident *op)
{
  return env_operator_precedence(&g_kc3_env, op);
}

s_ident * operator_resolve (const s_ident *op, u8 arity,
                            s_ident *dest)
{
  return env_operator_resolve(&g_kc3_env, op, arity, dest);
}

const s_sym * operator_symbol (const s_ident *op)
{
  return env_operator_symbol(&g_kc3_env, op);
}
