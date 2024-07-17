#include "MyTimer.h"


#include <sys/time.h>
#include <time.h>
#include <stddef.h>
#include <chrono>

using namespace std;

#ifndef WIN
#define WIN
#endif

#ifdef WIN
#include <chrono>
#endif

/*
 *  The data structure for getting time.
 */
static struct timeval tim;

/*
 *  Return the current time in double format.
 */
double getCurrentTime() {
#ifdef WIN
    long long time = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now().time_since_epoch()).count();
    return time / 1000000.0;
#else
    gettimeofday(&tim, NULL);
    return tim.tv_sec + (tim.tv_usec / (double) CLOCKS_PER_SEC);
#endif
}
