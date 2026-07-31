#ifndef LIBKC3_RUSAGE_H
#define LIBKC3_RUSAGE_H

#include "types.h"

u64 * rusage_current_rss_bytes (u64 *dest);
u64 * rusage_max_rss_bytes (u64 *dest);
u64 * rusage_open_files (u64 *dest);
s_time * rusage_system_cpu_time (s_time *dest);
s_time * rusage_user_cpu_time (s_time *dest);
u64 * rusage_virtual_bytes (u64 *dest);

#endif
