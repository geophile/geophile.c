#ifndef _STOPWATCH_H
#define _STOPWATCH_H

#include <sys/time.h>

namespace geophile
{
    class Stopwatch
    {
    public:
        Stopwatch()
        {
            reset();
        }

        void start()
        {
            gettimeofday(&_start, NULL);
        }

        void stop()
        {
            gettimeofday(&_stop, NULL);
            _usec += 
                (_stop.tv_sec - _start.tv_sec) * 1000000 + 
                (_stop.tv_usec - _start.tv_usec);
        }

        void reset()
        {
            _usec = 0;
            start();
        }

        uint64_t usec()
        {
            return _usec;
        }

    private:
        struct timeval _start;
        struct timeval _stop;
        uint64_t _usec;
    };
}

#endif
