#include "system_init.h"
#include "debug.h"
#include "common_timer.h"


int main(void)
{
    system_init();
    uint32_t counter = 0;
    while (1)
    {
        counter++;
        ctmr_delay(1000);
        DEBUG_PRINT("c=%lu", counter);
    }
}
