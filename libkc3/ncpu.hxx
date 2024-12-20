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
/**
 * @file ncpu.hh
 * @brief Number of CPUs and threads.
 *
 * Function that returns the number of CPU cores and threads.
 */
#ifndef LIBKC3_NCPU_HXX
#define LIBKC3_NCPU_HXX

extern "C" {

#include "types.h"

  sw ncpu (void);

}

#endif /* LIBKC3_NCPU_HXX */
