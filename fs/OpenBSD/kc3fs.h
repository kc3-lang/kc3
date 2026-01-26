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
#ifndef KC3FS_KC3FS_H
#define KC3FS_KC3FS_H

#if ! defined(_KERNEL) && ! defined(_KMEMUSER)
# error "not supposed to be exposed to userland"
#endif

#include "../../libkc3/types.h"

/* Type definitions. */

typedef struct kc3fs_mem_dirent s_kc3fs_mem_dirent;
typedef struct kc3fs_mem_node   s_kc3fs_mem_node;

/* Enums. */

typedef enum kc3fs_type {
  KC3FS_VOID = 0,
  KC3FS_MEM = 1
} e_kc3fs_type;

/* 1 */

struct kc3fs_mem_node {
  u8 type;
  s_str *name;
  s_str *content;
};

struct kc3fs_mem_dirent {
  u8 type;
  kc3fs_mem_node *node;
  s_str *name;
}

#endif /* KC3_FS_TYPES_H */
