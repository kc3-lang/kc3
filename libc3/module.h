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
/**
 * @file module.h
 * @brief Module
 *
 * Structure to manipulate modules.
 * A module contains symbols which have a type and an optional value.
 */
#ifndef MODULE_H
#define MODULE_H

s_module * module_load (s_module *module, const s_sym *name,
                        s_facts *facts);

#endif /* MODULE_H */
