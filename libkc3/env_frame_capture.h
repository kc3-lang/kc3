/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_ENV_FRAME_CAPTURE_H
#define LIBKC3_ENV_FRAME_CAPTURE_H

#include "types.h"

s_frame * env_frame_capture_array (s_env *env, s_frame *frame,
                                   s_array *array);
s_frame * env_frame_capture_do_block (s_env *env, s_frame *frame,
                                      s_do_block *do_block);
s_frame * env_frame_capture_call (s_env *env, s_frame *frame,
                                  s_call *call);
s_frame * env_frame_capture_callable (s_env *env, s_frame *frame,
                                      s_callable *callable);
s_frame * env_frame_capture_fact (s_env *env, s_frame *frame,
                                  s_fact *fact);
s_frame * env_frame_capture_fn (s_env *env, s_frame *frame, s_fn *fn);
s_frame * env_frame_capture_ident (s_env *env, s_frame *frame,
                                   s_ident *ident);
s_frame * env_frame_capture_list (s_env *env, s_frame *frame,
                                  s_list *list);
s_frame * env_frame_capture_map (s_env *env, s_frame *frame,
                                 s_map *map);
s_frame * env_frame_capture_ptag (s_env *env, s_frame *frame,
                                  s_tag *ptag);
s_frame * env_frame_capture_quote (s_env *env, s_frame *frame,
                                   s_quote *quote);
s_frame * env_frame_capture_struct (s_env *env, s_frame *frame,
                                    s_struct *s);
s_frame * env_frame_capture_struct_type (s_env *env, s_frame *frame,
                                         s_struct_type *struct_type);
s_frame * env_frame_capture_tag (s_env *env, s_frame *frame,
                                 s_tag *tag);
s_frame * env_frame_capture_tuple (s_env *env, s_frame *frame,
                                   s_tuple *tuple);
s_frame * env_frame_capture_unquote (s_env *env, s_frame *frame,
                                     s_unquote *unquote);
s_frame * env_frame_capture_var (s_env *env, s_frame *frame,
                                 s_var *var);

#endif /* LIBKC3_ENV_FRAME_CAPTURE_H */
