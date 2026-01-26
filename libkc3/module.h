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
/**
 * @file module.h
 * @brief Module
 *
 * Structure to manipulate modules.
 * A module contains symbols which have a type and an optional value.
 */
#ifndef LIBKC3_MODULE_H
#define LIBKC3_MODULE_H

#include "types.h"

/* Modifiers */
bool module_ensure_loaded_ (const s_sym *module);
bool module_load (const s_sym *module);
bool module_maybe_reload_ (const s_sym *module);

/* Observers */
bool *          module_has_ident (const s_sym *module,
                                  const s_ident *ident,
                                  bool *dest);
bool *          module_has_symbol (const s_sym *module,
                                   const s_sym *sym,
                                   bool *dest);
bool *          module_is_loading (const s_sym *module, bool *dest);
const s_time ** module_load_time (const s_sym *module,
                                  const s_time **dest);
s_str *         module_path (const s_sym *module, const s_str *prefix,
                             const char *ext, s_str *dest);
sw              module_path_size (const s_sym *module,
                                  const s_str *prefix,
                                  const char *ext);
const s_sym *   module_to_sym (const s_sym *module);

#endif /* LIBKC3_MODULE_H */
