#include "system_init.h"
#include "debug.h"
#include "common_timer.h"
#include "cmsis_os.h"
#include "threads.h"


int main(void)
{
    system_init();
    osKernelInitialize();

    threads_start();

    osKernelStart();

    while (1)
    {
    }
}
