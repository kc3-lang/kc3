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
 * @file std.hxx
 * @brief Standard C++ library.
 *
 * Functions of the C++ std:: library.
 */
#ifndef LIBKC3_STD_HXX
#define LIBKC3_STD_HXX

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

sw std_thread_hardware_concurrency (void);

#ifdef __cplusplus
}
#endif

#endif /* LIBKC3_STD_HXX */
