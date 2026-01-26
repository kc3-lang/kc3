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
#ifndef KC3_FS_TYPES_H
#define KC3_FS_TYPES_H

#include "../../libkc3/types.h"

/* Type definitions. */

typedef struct mfs s_mfs;

/* Enums. */

typedef enum fs_node_type {
  FS_NODE_VOID = 0,
  FS_NODE_MFS = 1
} e_fs_node_type;

/* 1 */

struct mfs_node {
  e_fs_node_type type;
  s_str path;
  s_str content;
};

#endif /* KC3_FS_TYPES_H */
