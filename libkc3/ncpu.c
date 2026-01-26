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
#include <string.h>
#include <sys/types.h>

#if (defined(__APPLE__) ||      \
     defined(__FreeBSD__) ||    \
     defined(__NetBSD__) ||     \
     defined(__OpenBSD__))
# include <sys/sysctl.h>
#else
# if ! (defined(WIN32) || defined(WIN64))
#  include <sys/sysinfo.h>
# endif
#endif

#include "assert.h"
#include "io.h"
#include "ncpu.h"
#include "tag.h"

s_tag * ncpu_tag (s_tag *ncpu)
{
#if ! HAVE_PTHREAD
  tag_init_u8(ncpu, 1);
#else
# if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  {
    s32 mib[2];
    s32 hw_ncpu;
    uw len;
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(hw_ncpu);
    if (sysctl(mib, 2, &hw_ncpu, &len, NULL, 0) == -1){
      err_puts("env_globals_init: sysctl(hw.ncpu)");
      assert(! "env_globals_init: sysctl(hw.ncpu)");
      return NULL;
    }
    tag_init_s32(ncpu, hw_ncpu);
    tag_integer_reduce(ncpu, ncpu);
  }
# elif defined(HAVE_ANDROID)
  tag_init_s32(ncpu, sysconf(_SC_NPROCESSORS_ONLN));
# else
  tag_init_s32(ncpu, get_nprocs());
# endif
#endif
  return ncpu;
}

u32 ncpu_u32 (void)
{
#if ! HAVE_PTHREAD
  return 1;
#else
# if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
  {
    s32 mib[2];
    s32 hw_ncpu;
    uw len;
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(hw_ncpu);
    if (sysctl(mib, 2, &hw_ncpu, &len, NULL, 0) == -1){
      err_puts("ncpu_u32: sysctl: hw.ncpu");
      assert(! "ncpu_u32: sysctl: hw.ncpu");
      return 1;
    }
    if (hw_ncpu <= 0) {
      err_puts("ncpu_u32: sysctl: hw.ncpu <= 0");
      assert(! "ncpu_u32: sysctl: hw.ncpu <= 0");
      return 1;
    }
    return hw_ncpu;
  }
# elif defined(HAVE_ANDROID)
  return sysconf(_SC_NPROCESSORS_ONLN);
# else
  return get_nprocs();
# endif
#endif
}
