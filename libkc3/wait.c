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
#ifdef WIN32

#include <stdlib.h>
#include "wait.h"

int __filter_anychild (PROCESSENTRY32W * pe, DWORD pid)
{
    (void) pid;
    return pe->th32ParentProcessID == GetCurrentProcessId();
}

int __filter_pid (PROCESSENTRY32W * pe, DWORD pid)
{
    return pe->th32ProcessID == pid;
}

/*
FIXME: causes FILETIME to conflict
static void __filetime2timeval(FILETIME time, struct timeval * out)
{
    unsigned long long value = time.dwHighDateTime;
    value = (value << 32) | time.dwLowDateTime;
    out->tv_sec = (long)(value / 1000000);
    out->tv_usec = (long)(value % 1000000);
}
*/

int __waitpid_internal (pid_t pid, int * status, int options,
			siginfo_t * infop, struct rusage * rusage)
{
    int saved_status = 0;
    HANDLE hProcess = INVALID_HANDLE_VALUE;
    HANDLE hSnapshot = INVALID_HANDLE_VALUE;
    int (*filter)(PROCESSENTRY32W*, DWORD);
    PROCESSENTRY32W pe;
    DWORD wait_status = 0, exit_code = 0;
    int nohang = WNOHANG == (WNOHANG & options);
    options &= ~(WUNTRACED | __WNOWAIT | __WCONTINUED | WNOHANG);
    if (options)
    {
        errno = -EINVAL;
        return -1;
    }

    if (pid == -1)
    {
        /* wait for any child */
        filter = __filter_anychild;
    }
    else if (pid < -1)
    {
        /* wait for any process from the group */
        abort(); /* not implemented */
    }
    else if (pid == 0)
    {
        /* wait for any process from the current group */
        abort(); /* not implemented */
    }
    else
    {
        /* wait for process with given pid */
        filter = __filter_pid;
    }

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        errno = ECHILD;
        return -1;
    }
    pe.dwSize = sizeof(pe);
    if (!Process32FirstW(hSnapshot, &pe))
    {
        CloseHandle(hSnapshot);
        errno = ECHILD;
        return -1;
    }
    do
    {
        if (filter(&pe, pid))
        {    
            hProcess = OpenProcess(SYNCHRONIZE |
                                   PROCESS_QUERY_INFORMATION,
                                   0, pe.th32ProcessID);
            if (INVALID_HANDLE_VALUE == hProcess)
            {
                CloseHandle(hSnapshot);
                errno = ECHILD;
                return -1;
            }
            break;
        }
    }
    while (Process32NextW(hSnapshot, &pe));
    if (INVALID_HANDLE_VALUE == hProcess)
    {
        CloseHandle(hSnapshot);
        errno = ECHILD;
        return -1;
    }

    wait_status = WaitForSingleObject(hProcess, nohang ? 0 : INFINITE);

    if (! wait_status)
    {
        if (GetExitCodeProcess(hProcess, &exit_code))
            saved_status |= (exit_code & 0xFF) << 8;
    }
    else if (WAIT_TIMEOUT == wait_status && nohang)
    {
        return 0;
    }
    else
    {
        CloseHandle(hProcess);
        CloseHandle(hSnapshot);
        errno = ECHILD;
        return -1;
    }
    if (rusage)
    {
        memset(rusage, 0, sizeof(*rusage));
        /*
        FIXME: causes FILETIME to conflict
        FILETIME creation_time, exit_time, kernel_time, user_time;
        if (GetProcessTimes(hProcess, &creation_time, &exit_time,
                            &kernel_time, &user_time))
        {
             __filetime2timeval(kernel_time, &rusage->ru_stime);
             __filetime2timeval(user_time, &rusage->ru_utime);
        }
        */
    }
    if (infop)
    {
        memset(infop, 0, sizeof(*infop));
    }

    CloseHandle(hProcess);
    CloseHandle(hSnapshot);

    if (status)
        *status = saved_status;

    return pe.th32ParentProcessID;
}

int waitpid(pid_t pid, int * status, int options)
{
    return __waitpid_internal(pid, status, options, NULL, NULL);
}

int wait(int *status)
{
    return __waitpid_internal(-1, status, 0, NULL, NULL);
}

int waitid(idtype_t idtype, id_t id, siginfo_t * infop, int options)
{
    pid_t pid;
    switch (idtype)
    {
    case P_PID: pid = id; break;
    case P_PGID: pid = -(pid_t)id; break;
    case P_ALL: pid = 0; break;
    default: errno = EINVAL; return -1;
    }
    return __waitpid_internal(pid, NULL, options, infop, NULL);
}

pid_t wait3(int * status, int options, struct rusage * rusage)
{
    return __waitpid_internal(-1, status, options, NULL, rusage);
}

pid_t wait4(pid_t pid, int * status, int options,
            struct rusage * rusage)
{
    return __waitpid_internal(pid, status, options, NULL, rusage);
}

#else

void _kc3_wait (void) {}

#endif
