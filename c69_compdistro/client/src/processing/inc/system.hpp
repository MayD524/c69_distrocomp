#pragma once

#ifndef SYSTEM_HPP
#define SYSTEM_HPP 

#ifdef _WIN32
#   include <windows.h>
#   include <conio.h>
#else
#   include <unistd.h>
#endif


int getSystemMemory() {
    #ifdef _WIN32
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof(statex);
        GlobalMemoryStatusEx(&statex);
        return statex.ullTotalPhys / 1024 / 1024;
    #else
        return 0;
    #endif
}

int getSystemCores() {
    #ifdef _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
    #else
        return 0;
    #endif
}

int getCPUUsagePercent() {
    #ifdef _WIN32
        FILETIME idleTime, kernelTime, userTime;
        GetSystemTimes(&idleTime, &kernelTime, &userTime);
        ULARGE_INTEGER k, u, i;
        k.LowPart = kernelTime.dwLowDateTime;
        k.HighPart = kernelTime.dwHighDateTime;
        u.LowPart = userTime.dwLowDateTime;
        u.HighPart = userTime.dwHighDateTime;
        i.LowPart = idleTime.dwLowDateTime;
        i.HighPart = idleTime.dwHighDateTime;
        ULONGLONG total = k.QuadPart + u.QuadPart;
        ULONGLONG idle = i.QuadPart;
        return (int)((total - idle) * 100 / total);
    #else
        return 0;
    #endif
}

#endif