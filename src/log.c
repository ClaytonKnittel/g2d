
#include <g2d/log.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#ifndef RELEASE_BUILD

const char* log_prio_prefixes[] = {
	// LOG_PRIO_CRITICAL
	"[CRIT]",
	// LOG_PRIO_DEBUG
	"[DBG]"
};

#endif /* RELEASE_BUILD */

