#include "timer.h"

void timer_init(void)
{
    TCCR1B |= (1 << CS12) | (1 << CS10); // Set up timer at Fcpu/1024
}

void wait_one_second(void (*f)(void))
{
    TCNT1 = 0;
    while (TCNT1 < 15625)
    {
        if (f)
            f();
    }
}