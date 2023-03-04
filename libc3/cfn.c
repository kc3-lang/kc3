/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include "cfn.h"

s_tag * cfn_apply (s_cfn *cfn, s_list *args) {
  ffi_type **arg_types;
  void **arg_values;
  ffi_cif cif;
  sw i;
  sw num_args;
  void* result;
  ffi_type* result_type;
  num_args = list_length(args);
  if (! num_args) {
    /* TODO */
    assert(! "todo");
    err(1, "todo");
    return NULL;
  }
  if (! (arg_types = malloc(sizeof(ffi_type *) * num_args)))
    err(1, "cfn_apply");
  if (! (arg_values = malloc(sizeof(void *) * num_args)))
    err(1, "cfn_apply");
  while (args) {
    switch (args->tag.type.type) {
    case TAG_S8:
      arg_types[num_args] = &ffi_type_sint;
      arg_values[num_args] = args->data;
      num_args++;
      break;
    case FLOAT:
      arg_types[num_args] = &ffi_type_float;
      arg_values[num_args] = args->data;
      num_args++;
      break;
    case STRING:
      arg_types[num_args] = &ffi_type_pointer;
      arg_values[num_args] = &args->data;
      num_args++;
      break;
    }

    args = args->next;
  }

  ffi_type** final_arg_types = malloc(sizeof(ffi_type*) * num_args);
  void** final_arg_values = malloc(sizeof(void*) * num_args);

  for (int i = 0; i < num_args; i++) {
    final_arg_types[i] = arg_types[i];
    final_arg_values[i] = arg_values[i];
  }

  ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI, num_args, NULL, final_arg_types);

  switch (((Node*)fn_ptr)->tag) {
  case INT:
    result_type = &ffi_type_sint;
    break;
  case FLOAT:
    result_type = &ffi_type_float;
    break;
  case STRING:
    result_type = &ffi_type_pointer;
    break;
  }

  ffi_call(&cif, ((Node*)fn_ptr)->data, result, final_arg_values);

  free(final_arg_types);
  free(final_arg_values);

  if (((Node*)fn_ptr)->tag == STRING) {
    char* result_copy = strdup(result);
    free_result(result, ((Node*)fn_ptr)->tag);
    result = result_copy;
  }

  return result;
}
#endif /* CFN_H */
