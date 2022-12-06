/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
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
#ifndef BINDING_H
#define BINDING_H

#include "types.h"

/* stack-allocation compatible functions */
void        binding_clean (s_binding *binding);
s_binding * binding_init (s_binding *binding, const s_sym *name,
                          const s_tag *value, s_binding *next);

/* constructors */
s_binding * binding_new (const s_sym *name, const s_tag *value,
                         s_binding *next);

/* destructors */
void binding_delete (s_binding *binding);
void binding_delete_all (s_binding *binding);

/* observers */
const s_tag * binding_get (s_binding *binding, const s_sym *name);

#endif /* BINDING_H */
