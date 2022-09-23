#ifndef _G2D_LOG_H
#define _G2D_LOG_H

#include <stdint.h>

#ifdef RELEASE_BUILD
// when in production build mode, change prints to syslogs

#include <syslog.h>

enum log_priority {
  ERR = LOG_ERR,
  WRN = LOG_WARNING,
  INF = LOG_INFO,
  DBG = LOG_DEBUG,
};

#define _do_log(LOG_LEVEL, fmt, ...) \
  syslog((int) (log_priority) LOG_LEVEL, fmt##__VA_ARGS__)

#else

#include <stdio.h>
#include <time.h>

#include <g2d/common/utils.h>

#define LOG_TIME_FMT      "%F %T %Z"
#define LOG_TIME_BUF_SIZE 24

enum log_priority {
  ERR = 0,
  WRN,
  INF,
  DBG,
};

extern const char* log_prio_prefixes[];

#define _do_log(log_level, fmt, ...)                                        \
  do {                                                                      \
    struct tm tm;                                                           \
    time_t cur_time = time(NULL);                                           \
    gmtime_r(&cur_time, &tm);                                               \
    char time_str[LOG_TIME_BUF_SIZE];                                       \
    strftime(time_str, LOG_TIME_BUF_SIZE, LOG_TIME_FMT, &tm);               \
    fprintf(stderr, "%s %s " fmt "\n", time_str,                            \
            log_prio_prefixes[(uint32_t) (log_priority) log_level] VA_ARGS( \
                __VA_ARGS__));                                              \
  } while (0)

#endif /* RELEASE_BUILD */

#define g2dlog(log_level, fmt, ...) _do_log(log_level, fmt, __VA_ARGS__)

#endif /* _G2D_LOG_H */
