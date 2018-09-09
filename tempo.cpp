#ifndef TEMPO_CPP
#define TEMPO_CPP
#include<sys/time.h>
#include<sys/resource.h>
#include<time.h>
  
using namespace std;

double getsystime(void)
{
	double t;
	struct timeval tim;
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	tim=ru.ru_stime;
	t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0; 
	return t;
}


double getcputime(void)        
{ 
	struct timeval tim;        
	struct rusage ru;        
	getrusage(RUSAGE_SELF, &ru);        
	tim=ru.ru_utime;        
	double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;        
	//tim=ru.ru_stime;        
	//t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;        
	return t; 
}  



/*  clock_t clock(void) (include: <time.h>)
      Returns the amount of processor time actually used by the current
      process, measured in units of CLOCKS_PER_SEC, which on this system
      is 128.
         Example:
             clock_t t1=clock();
             do_something_long();
             clock_t t2=clock();
             printf("%.4lf seconds of processing\n", (t2-t1)/(double)CLOCKS_PER_SEC);
      The clock function provides a very simple interface, but
      does not distinguish between time spent on user code and time spent 
      executing system functions; it merely measures the total CPU time
      devoted so far to the current process. */


/*struct timeval (include: <sys/time.h>)
    The struct timeval structure represents an elapsed time. Members:
    long int tv_sec
        This represents the number of whole seconds of elapsed time.
    long int tv_usec
        This is the rest of the elapsed time (a fraction of a second), 
        represented as the number of microseconds. It is always less than one million. */

/*int getrusage(int target, rusage * results)
      include: <sys/time.h>
      include: <sys/resource.h>
      Not avalibale under DOS or Windows.
      Returns 0 for success, and negative for failure; there are no reasonable
      reasons for failure to occur.
      The target parameter must be one of the two pre-defined values
      RUSAGE_SELF or RUSAGE_CHILDREN. RUSAGE_SELF measures only the current
      process, RUSAGE_CHILDREN measures only subprocesses that have already
      terminated. Unix provides no useful way to obtain information for
      still-running subprocesses.
      The results parameter is a pointer to a struct that the calling
      program must create, and which will be filled in with statistics on
      resource information. The fields of this struct are as follows:
         (the first two fields are structs of type timeval, see below)
             ru_utime       time spent executing user code
             ru_stime       time spent executing system functions
         (the remaining fields are ints)
             ru_maxrss      max resident set size (physical memory allocated)
             ru_ixrss       memory size for code
             ru_idrss       memory size for statics, globals, and new/malloc
             ru_isrss       stack size (memory used by local variables)
             ru_minflt      minor page faults: "page reclaims"
             ru_majflt      major page faults: swaps in
             ru_nswap       page swaps
             ru_inblock     block input operations, disc etc
             ru_oublock     block output operations, disc, etc
             ru_msgsnd      messages sent
             ru_msgrcv      messages received
             ru_nsignals    signals received
             ru_nvcsw       voluntary context switches (process loses CPU)
             ru_nivcsw      involuntary context switches (process loses CPU)
      The timeval struct used to measure time has only two fields, and
      both are unsigned ints. They are named tv_sec and tv_usec, and
      jointly represent one single value. tv_sec*1000000+tv_usec gives the
      number of microseconds.
         Example:
             double getcputime(void)        
             { struct timeval tim;        
               struct rusage ru;        
               getrusage(RUSAGE_SELF, &ru);        
               tim=ru.ru_utime;        
               double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;        
               tim=ru.ru_stime;        
               t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;        
               return t; }        
      This example function returns the total CPU time consumed by the current
      process, measured in seconds, as a double precision floating point number.
      It adds together the user time and the system time.
      Note: Although the format used is capable of measuring time to an accuracy
      of a microsecond, do not expect that much precision from any real system.*/

#endif
