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
#ifndef LIBKC3_FACTS_SPEC_H
#define LIBKC3_FACTS_SPEC_H

#include "types.h"

/* facts specification
   ===================

   A spec has the following pattern :
    (subject (predicate object)+ NULL)+ NULL
   For instance one fact {s1, p1.1, o1.1} has the spec :
     s1 p1.1 o1.1 NULL NULL
   And four facts of two subjects, each having two properties yield
   this spec :
     s1 p1.1 o1.1 p1.2 o1.2 NULL s2 p2.1 o2.1 p2.2 o2.2 NULL NULL
*/

/* Constructors */
p_facts_spec facts_spec_new_expand (p_facts_spec spec);
p_facts_spec facts_spec_new_list (s_list *spec);

/* Destructor */
#define facts_spec_delete free

/* Observers */
uw facts_spec_count_facts (p_facts_spec spec);

/* Modifiers */
p_facts_spec facts_spec_sort (p_facts_spec spec);

#endif /* LIBKC3_FACTS_SPEC_H */
