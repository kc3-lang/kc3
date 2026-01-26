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
 * @file fn_clause.h
 * Function clause type.
 *
 * Syntax : "fn" (type? "(" ((type space)? sym ","?)* ")" "{" prog "}")+
 */
#ifndef LIBKC3_FN_CLAUSE_H
#define LIBKC3_FN_CLAUSE_H

#include "types.h"

/* Stack-allocation compatible functions, call fn_clause_clean
   after use. */
void          fn_clause_clean (s_fn_clause *clause);
s_fn_clause * fn_clause_init (s_fn_clause *clause,
                              s_fn_clause *next_clause);

/* Heap-allocation functions, call fn_clause_delete* after use. */
s_fn_clause * fn_clause_delete (s_fn_clause *clause);
void          fn_clause_delete_all (s_fn_clause *clause);
s_fn_clause * fn_clause_new (s_fn_clause *next_clause);
s_fn_clause * fn_clause_new_copy (s_fn_clause *src);

#endif /* LIBKC3_FN_CLAUSE_H */
