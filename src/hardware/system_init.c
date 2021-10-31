#include "system_init.h"
#include "mcu.h"
#include "stm32f103xb.h"
#include "global_inc.h"
#include "interrupts.h"
#include "common_timer.h"

#define _INIT_CLK_TIMEOUT_US    (10000)

static void s_system_clock_init(void);
static void s_set_pll_sys_clk(void);


static void s_wait_clock_enable(uint32_t clk_rdy_bit)
{
    uint16_t timer = ctmr_start_timer(_INIT_CLK_TIMEOUT_US);
    while (!(RCC->CR & clk_rdy_bit) && !ctmr_is_timer_expired(timer))
    {
    }

    if (ctmr_is_timer_expired(timer))
    {
        //todo print error
        while (1)
        {
        }
    }
}

static void s_set_pll_sys_clk(void)
{
    RCC->CR |= RCC_CR_HSEON;

    s_wait_clock_enable(RCC_CR_HSERDY);

    //latency for 72 MHz
    FLASH->ACR |= FLASH_ACR_LATENCY_2;

    //init 72 MHz for 8 MHz HSE
    //RCC_CFGR_HPRE_DIV1 AHB prescaler = 1 for 72 MHz(MAX)
    //RCC_CFGR_PPRE1_DIV2 APB1 prescaler = 2 for 36 MHz (MAX)
    //RCC_CFGR_PPRE2_DIV1 APB2 prescaler = 1 for 72 MHz(MAX)
    //RCC_CFGR_ADCPRE_DIV6 ADC prescaler = 6 for 12 MHz (14 MHz MAX)
    //RCC_CFGR_PLLXTPRE_HSE Select HSE for PLL without prescaler
    //RCC_CFGR_PLLMULL9 8 * 9 == 72 MHz set MAX clock
    //(0 << RCC_CFGR_USBPRE_Pos) - clock for usb == 72 / 1.5 = 48 MHz
    //RCC_CFGR_MCO_NOCLOCK - No pin for out clock
    RCC->CFGR = RCC_CFGR_SW_PLL |
                  RCC_CFGR_HPRE_DIV1 |
                  RCC_CFGR_PPRE1_DIV2 |
                  RCC_CFGR_PPRE2_DIV1 |
                  RCC_CFGR_ADCPRE_DIV6 |
                  RCC_CFGR_PLLSRC |
                  RCC_CFGR_PLLXTPRE_HSE |
                  RCC_CFGR_PLLMULL9 |
                  (0 << RCC_CFGR_USBPRE_Pos) |
                  RCC_CFGR_MCO_NOCLOCK;

    RCC->CR |= RCC_CR_PLLON;
    s_wait_clock_enable(RCC_CR_PLLRDY);

    RCC->CR &= ~RCC_CR_HSION;
}

static void s_system_clock_init(void)
{

    ctmr_init_ms(RESET_SYSTEM_CLOCK);

    s_set_pll_sys_clk();

    ctmr_init_ms(SYSTEM_CLOCK);
}

#ifdef DEBUG
void debug_init(void)
{
    uint32_t port_mask = 1u;
    DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN | DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY;
    ITM->LAR = 0xC5ACCE55;


    ITM->TCR |= ITM_TCR_ITMENA_Msk | ITM_TCR_DWTENA_Msk | (port_mask << ITM_TCR_TraceBusID_Pos);
    ITM->TER |= port_mask;
    ITM->TPR |= port_mask;
}
#endif

void system_init(void)
{

#ifdef DEBUG
    debug_init();
#endif
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    interrupts_init();
    s_system_clock_init();
}
