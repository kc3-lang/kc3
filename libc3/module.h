/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_MODULE_H
#define LIBC3_MODULE_H

#include "types.h"

/* Modifiers */
bool module_ensure_loaded (const s_sym *module, s_facts *facts);
bool module_load (const s_sym *module, s_facts *facts);
bool module_maybe_reload (const s_sym *module, s_facts *facts);

/* Observers */
bool          module_has_ident (const s_sym *module,
                                const s_ident *ident,
                                s_facts *facts);
bool          module_has_symbol (const s_sym *module,
                                 const s_sym *sym,
                                 s_facts *facts);
bool          module_is_loading (const s_sym *module);
s_tag *       module_load_time (const s_sym *module, s_facts *facts,
                                s_tag *dest);
s_str *       module_path (const s_sym *module, const s_str *prefix,
                           const char *ext, s_str *dest);
sw            module_path_size (const s_sym *module,
                                const s_str *prefix,
                                const char *ext);
const s_sym * module_to_sym (const s_sym *module);

#endif /* LIBC3_MODULE_H */
