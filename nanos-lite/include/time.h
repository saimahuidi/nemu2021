#include <common.h>
#include <am.h>
#include <amdev.h>
struct timeval
{
#ifdef __USE_TIME_BITS64
  __time64_t tv_sec;		/* Seconds.  */
  __suseconds64_t tv_usec;	/* Microseconds.  */
#else
  long tv_sec;		/* Seconds.  */
  long tv_usec;	/* Microseconds.  */
#endif
};

struct timezone
  {
    int tz_minuteswest;		/* Minutes west of GMT.  */
    int tz_dsttime;		/* Nonzero if DST is ever in effect.  */
  };

int gettimeofday(struct timeval *restrict tv, void *restrict tz);