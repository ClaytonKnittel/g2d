#ifndef _G2D_LOG_H
#define _G2D_LOG_H

#ifdef RELEASE_BUILD
// when in production build mode, change prints to syslogs

#include <syslog.h>

enum log_priority {
	LOG_PRIO_CRITICAL = LOG_CRIT,
	LOG_PRIO_DEBUG = LOG_DEBUG
};

#define _do_log(LOG_LEVEL, fmt, ...) syslog(LOG_LEVEL, fmt ##__VA_ARGS__)

#else

#define LOG_TIME_FMT "%F %T %Z"
#define LOG_TIME_BUF_SIZE 24

enum log_priority {
	LOG_PRIO_CRITICAL = 0,
	LOG_PRIO_DEBUG
};

extern const char* log_prio_prefixes[];

#define _do_log(priority, fmt, ...) \
	do { \
		struct tm tm; \
		time_t cur_time = time(NULL); \
		gmtime_r(&cur_time, &tm); \
		char time_str[LOG_TIME_BUF_SIZE]; \
		strftime(time_str, LOG_TIME_BUF_SIZE, LOG_TIME_FMT, &tm); \
		fprintf(stderr, "%s %s " fmt "\n", time_str, log_prio_prefixes[priority] ##__VA_ARGS__); \
	} while (0)

#endif /* RELEASE_BUILD */

#define log_info(fmt, ...) _do_log(LOG_PRIO_DEBUG, fmt ##__VA_ARGS__)
#define log_error(fmt, ...) _do_log(LOG_PRIO_CRITICAL, fmt ##__VA_ARGS__)

#endif /* _G2D_LOG_H */
