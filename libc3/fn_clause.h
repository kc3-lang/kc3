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
 * @file fn_clause.h
 * Function clause type.
 *
 * Syntax : "fn" (type? "(" ((type space)? sym ","?)* ")" "{" prog "}")+
 */
#ifndef LIBC3_FN_CLAUSE_H
#define LIBC3_FN_CLAUSE_H

#include "types.h"

/* stack-allocation compatible functions */
void          fn_clause_clean (s_fn_clause *clause);
s_fn_clause * fn_clause_init (s_fn_clause *clause, s_fn_clause *next_clause);

/* constructors */
s_fn_clause * fn_clause_new (s_fn_clause *next_clause);
s_fn_clause * fn_clause_new_copy (const s_fn_clause *src);

/* destructors */
s_fn_clause * fn_clause_delete (s_fn_clause *clause);
void          fn_clause_delete_all (s_fn_clause *clause);

#endif /* LIBC3_FN_H */
