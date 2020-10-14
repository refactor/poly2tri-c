#pragma once


#ifdef __APPLE__
#include <pthread.h>
#define thrd_id() ({\
        uint64_t tid;\
        pthread_threadid_np(NULL, &tid);\
        tid;\
        })
#else

#define _GNU_SOURCE

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdint.h>

uint64_t thrd_id() {
    return (uint64_t)syscall(SYS_gettid);
}
#endif

#include <inttypes.h>
#include <stdio.h>

#ifndef LOGLEVEL
#define LOGLEVEL LOGLEVEL_DEBUG
#endif

enum {
    LOGLEVEL_CRITICAL,
    LOGLEVEL_ERROR,
    LOGLEVEL_WARNING,
    LOGLEVEL_INFO,
    LOGLEVEL_DEBUG,
    LOGLEVEL_NONE
};

const char * log_level_strings [] = { "ERRO", "WARN", "INFO", "DEBG", "NONE" };


#define LOGLEVEL_DEBUG_COLOR(str)    "\033[30;1m" str "\033[0m"
#define LOGLEVEL_INFO_COLOR(str)     "\033[37;1m" str "\033[0m"
#define LOGLEVEL_WARNING_COLOR(str)  "\033[1;35m" str "\033[0m"
#define LOGLEVEL_ERROR_COLOR(str)    "\033[1;31m" str "\033[0m"
#define LOGLEVEL_CRITICAL_COLOR(str) "\033[1;31m" str "\033[0m"

#define LOG(level, fmt, ...) \
    do { if (level <= LOGLEVEL) fprintf(stdout, level##_COLOR("[%34s#%-5d@(tid:%"PRIx64")]") " " fmt "\r\n", __FUNCTION__,__LINE__,thrd_id(), ##__VA_ARGS__); } while(0)


#define DBG(fmt, ...)  LOG(LOGLEVEL_DEBUG,   fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) LOG(LOGLEVEL_INFO,    fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) LOG(LOGLEVEL_WARNING, fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)  LOG(LOGLEVEL_ERROR,   fmt, ##__VA_ARGS__)
#define CRI(fmt, ...)  LOG(LOGLEVEL_CRITICAL,fmt, ##__VA_ARGS__)



