
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <g2d/common/log.h>

#ifndef RELEASE_BUILD

const char* log_prio_prefixes[] = {
  "[ERR]",
  "[WRN]",
  "[INF]",
  "[DBG]",
};

#endif /* RELEASE_BUILD */
