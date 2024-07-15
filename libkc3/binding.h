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
#ifndef LIBC3_BINDING_H
#define LIBC3_BINDING_H

#include "types.h"

/* stack-allocation compatible functions */
void        binding_clean (s_binding *binding);
s_binding * binding_init (s_binding *binding, const s_sym *name,
                          const s_tag *value, s_binding *next);

/* constructors */
s_binding * binding_new (const s_sym *name, const s_tag *value,
                         s_binding *next);

/* destructors */
s_binding * binding_delete (s_binding *binding);
void        binding_delete_all (s_binding *binding);

/* observers */
const s_tag * binding_get (const s_binding *binding, const s_sym *name);
const s_tag * binding_is_bound (const s_binding *binding,
                                const s_sym *name);

#endif /* LIBC3_BINDING_H */
