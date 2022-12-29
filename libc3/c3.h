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
#ifndef C3_H
#define C3_H

#include "arg.h"
#include "bool.h"
#include "buf.h"
#include "buf_file.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "buf_save.h"
#include "call.h"
#include "character.h"
#include "compare.h"
#include "debug.h"
#include "env.h"
#include "eval.h"
#include "fact.h"
#include "facts.h"
#include "fn.h"
#include "hash.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "ptr.h"
#include "quote.h"
#include "str.h"
#include "tag.h"
#include "tuple.h"
#include "ucd.h"

/* libc3 */
void libc3_init ();
void libc3_shutdown ();

/* c3 */
s_module * c3_init (s_module *c3, s_facts *facts);
void       c3_clean (s_module *c3);

#endif /* C3_H */
