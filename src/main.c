#include "system_init.h"
#include "debug.h"
#include "common_timer.h"
#include "cmsis_os.h"

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes =
{.name = "defaultTask", .priority = (osPriority_t) osPriorityNormal,
        .stack_size = 128 * 4};

void StartDefaultTask(void *argument)
{
    uint32_t counter = 0;
    for (;;)
    {
        counter++;
        ctmr_delay(1000);
        DEBUG_PRINT("c=%lu", counter);
        osDelay(1);
    }

}

int main(void)
{
    system_init();
    osKernelInitialize();
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL,
            &defaultTask_attributes);
    osKernelStart();

    while (1)
    {

    }
}
