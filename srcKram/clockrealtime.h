#ifndef CLOCKREALTIME_H
#define CLOCKREALTIME_H
#include <time.h>

class ClockRealTime
{
public:
    ClockRealTime() {start();}

    void start();
    double elapsed();
    double interval();
    static double realTime();
    static double diff(struct timespec &start, struct timespec &stop);

private:
    struct timespec m_start;
    struct timespec m_stop;
};

#endif // CLOCKREALTIME_H
