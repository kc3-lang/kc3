#ifndef LIBKC3_RUSAGE_H
#define LIBKC3_RUSAGE_H

#include "types.h"

u64 * rusage_current_rss_bytes (u64 *dest);
u64 * rusage_max_rss_bytes (u64 *dest);
u64 * rusage_open_files (u64 *dest);
f64 * rusage_system_cpu_seconds (f64 *dest);
f64 * rusage_user_cpu_seconds (f64 *dest);
u64 * rusage_virtual_bytes (u64 *dest);

#endif
