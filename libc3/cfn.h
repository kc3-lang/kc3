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
#ifndef CFN_H
#define CFN_H

#include "types.h"

/* stack-allocation compatible functions */
s_cfn * cfn_init (s_cfn *cfn);
void    cfn_clean (s_cfn *cfn);

/* observers */
s_tag * cfn_apply (s_cfn *cfn, s_list *args, s_tag *dest);
s_cfn * cfn_copy (const s_cfn *cfn, s_cfn *dest);

/* modifiers */
s_cfn * cfn_link (s_cfn *cfn);

#endif /* CFN_H */
