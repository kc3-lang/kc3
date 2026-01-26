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
#include "assert.h"
#include "rpc.h"
#include "buf.h"
#include "env.h"
#include "eval.h"
#include "str.h"
#include "struct.h"
#include "sym.h"
#include "tag.h"

s_tag * rpc_request (const s_str *input, s_tag *dest)
{
  s_env *env;
  s_buf *env_err;
  s_buf *env_out;
  s_buf err_buf;
  char  err_buf_data[BUF_SIZE];
  s_buf out_buf;
  char  out_buf_data[BUF_SIZE];
  s_tag input_tag;
  s_rpc_response *response;
  s_tag tmp = {0};
  assert(input);
  assert(dest);
  env = env_global();
  assert(env);
  env_err = env->err;
  buf_init(&err_buf, false, sizeof(err_buf_data), err_buf_data);
  env->err = &err_buf;
  env_out = env->out;
  buf_init(&out_buf, false, sizeof(out_buf_data), out_buf_data);
  env->out = &out_buf;
  if (! tag_init_from_str(&input_tag, input)) {
    env->err = env_err;
    env->out = env_out;
    err_puts("rpc_request: invalid input");
    assert(! "rpc_request: invalid input");
    return NULL;
  }
  if (! tag_init_pstruct(&tmp, &g_sym_RPC_Response)) {
    env->err = env_err;
    env->out = env_out;
    err_puts("rpc_request: tag_init_pstruct %RPC.Response{}");
    assert(! "rpc_request: tag_init_pstruct %RPC.Response{}");
    tag_clean(&input_tag);
    return NULL;
  }
  if (! struct_allocate(tmp.data.pstruct)) {
    env->err = env_err;
    env->out = env_out;
    err_puts("rpc_request: struct_allocate");
    assert(! "rpc_request: struct_allocate");
    tag_clean(&tmp);
    tag_clean(&input_tag);
    return NULL;
  }
  response = tmp.data.pstruct->data;
  eval_tag(&input_tag, &response->result);
  if (buf_read_to_str(&err_buf, &response->err) < 0 ||
      buf_read_to_str(&out_buf, &response->out) < 0) {
    env->err = env_err;
    env->out = env_out;
    err_puts("rpc_request: buf_read_to_str");
    assert(! "rpc_request: buf_read_to_str");
    tag_clean(&tmp);
    tag_clean(&input_tag);
    return NULL;
  }
  tag_clean(&input_tag);
  env->err = env_err;
  env->out = env_out;
  *dest = tmp;
  return dest;
}

void rpc_response_clean (s_rpc_response *response)
{
  assert(response);
  str_clean(&response->err);
  str_clean(&response->out);
  tag_clean(&response->result);
}

