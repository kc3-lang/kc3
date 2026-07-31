#include <stdio.h>

#if ! (defined(WIN32) || defined(WIN64))
# include <fcntl.h>
# include <sys/resource.h>
# include <unistd.h>
#endif

#if defined(__OpenBSD__)
# include <sys/sysctl.h>
#endif

#include "rusage.h"

#if ! (defined(WIN32) || defined(WIN64))
static bool rusage_get (struct rusage *usage)
{
  return getrusage(RUSAGE_SELF, usage) == 0;
}
#endif

static bool rusage_memory (u64 *rss_bytes, u64 *virtual_bytes)
{
#if defined(__OpenBSD__)
  struct kinfo_proc process;
  size_t size;
  int mib[6];
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PID;
  mib[3] = getpid();
  mib[4] = sizeof(process);
  mib[5] = 1;
  size = sizeof(process);
  if (sysctl(mib, 6, &process, &size, NULL, 0) == -1)
    return false;
  *rss_bytes = (u64) process.p_vm_rssize * (u64) getpagesize();
  *virtual_bytes = ((u64) process.p_vm_tsize +
                    (u64) process.p_vm_dsize +
                    (u64) process.p_vm_ssize) * (u64) getpagesize();
  return true;
#elif defined(__linux__)
  FILE *f;
  unsigned long long resident_pages;
  unsigned long long virtual_pages;
  long page_size;
  if (! (f = fopen("/proc/self/statm", "r")))
    return false;
  if (fscanf(f, "%llu %llu", &virtual_pages, &resident_pages) != 2) {
    fclose(f);
    return false;
  }
  fclose(f);
  if ((page_size = sysconf(_SC_PAGESIZE)) <= 0)
    return false;
  *rss_bytes = (u64) resident_pages * (u64) page_size;
  *virtual_bytes = (u64) virtual_pages * (u64) page_size;
  return true;
#else
  (void) rss_bytes;
  (void) virtual_bytes;
  return false;
#endif
}

u64 * rusage_current_rss_bytes (u64 *dest)
{
  u64 virtual_bytes;
  if (! rusage_memory(dest, &virtual_bytes))
    return NULL;
  return dest;
}

u64 * rusage_max_rss_bytes (u64 *dest)
{
#if ! (defined(WIN32) || defined(WIN64))
  struct rusage usage;
  if (! rusage_get(&usage))
    return NULL;
# if defined(__APPLE__)
  *dest = usage.ru_maxrss;
# else
  *dest = (u64) usage.ru_maxrss * 1024;
# endif
  return dest;
#else
  (void) dest;
  return NULL;
#endif
}

u64 * rusage_open_files (u64 *dest)
{
#if defined(__OpenBSD__)
  *dest = getdtablecount();
  return dest;
#elif ! (defined(WIN32) || defined(WIN64))
  int fd;
  int size;
  u64 count;
  count = 0;
  size = getdtablesize();
  fd = 0;
  while (fd < size) {
    if (fcntl(fd, F_GETFD) != -1)
      count++;
    fd++;
  }
  *dest = count;
  return dest;
#else
  (void) dest;
  return NULL;
#endif
}

f64 * rusage_system_cpu_seconds (f64 *dest)
{
#if ! (defined(WIN32) || defined(WIN64))
  struct rusage usage;
  if (! rusage_get(&usage))
    return NULL;
  *dest = (f64) usage.ru_stime.tv_sec +
    (f64) usage.ru_stime.tv_usec / 1000000.0;
  return dest;
#else
  (void) dest;
  return NULL;
#endif
}

f64 * rusage_user_cpu_seconds (f64 *dest)
{
#if ! (defined(WIN32) || defined(WIN64))
  struct rusage usage;
  if (! rusage_get(&usage))
    return NULL;
  *dest = (f64) usage.ru_utime.tv_sec +
    (f64) usage.ru_utime.tv_usec / 1000000.0;
  return dest;
#else
  (void) dest;
  return NULL;
#endif
}

u64 * rusage_virtual_bytes (u64 *dest)
{
  u64 rss_bytes;
  if (! rusage_memory(&rss_bytes, dest))
    return NULL;
  return dest;
}
