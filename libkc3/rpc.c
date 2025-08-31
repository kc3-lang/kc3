/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "assert.h"
#include "rpc.h"
#include "buf.h"
#include "env.h"
#include "eval.h"
#include "str.h"
#include "sym.h"
#include "tag.h"

s_tag * rpc_request (const s_str *input, s_tag *dest)
{
  s_env *env;
  s_buf *env_err;
  s_buf *env_out;
  s_buf err_buf;
  s_buf out_buf;
  s_tag input_tag;
  s_rpc_response response = {0};
  assert(input);
  assert(dest);
  env = env_global();
  if (! env)
    return NULL;
  env_err = env->err;
  env_out = env->out;
  if (! buf_init_alloc(&err_buf, BUF_SIZE)) {
    return NULL;
  }
  if (! buf_init_alloc(&out_buf, BUF_SIZE)) {
    buf_clean(&err_buf);
    return NULL;
  }
  env->err = &err_buf;
  env->out = &out_buf;
  if (! tag_init_from_str(&input_tag, input)) {
    buf_clean(&err_buf);
    buf_clean(&out_buf);
    env->err = env_err;
    env->out = env_out;
    return NULL;
  }
  tag_init(&response.result);
  if (! eval_tag(&input_tag, &response.result)) {
    tag_init_void(&response.result);
  }
  if (buf_read_to_str(&err_buf, &response.err) < 0 ||
      buf_read_to_str(&out_buf, &response.out) < 0 ||
      ! tag_init_pstruct_copy_data(dest, &g_sym_RPC_Response,
                                   &response))
    goto clean;
  tag_clean(&input_tag);
  str_clean(&response.err);
  str_clean(&response.out);
  tag_clean(&response.result);
  buf_clean(&err_buf);
  buf_clean(&out_buf);
  env->err = env_err;
  env->out = env_out;
  return dest;
 clean:
  tag_clean(&input_tag);
  tag_clean(&response.result);
  str_clean(&response.err);
  str_clean(&response.out);
  buf_clean(&err_buf);
  buf_clean(&out_buf);
  env->err = env_err;
  env->out = env_out;
  return NULL;
}
