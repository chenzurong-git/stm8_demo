#include "stm8s_tim4.h"
#include "bsp_sys_pub.h"

volatile uint32_t system_ticks = 0;   // 系统tick计数
volatile uint32_t system_time_ms = 0; // 系统时间（毫秒）
volatile uint32_t system_time_s = 0;  // 系统时间（秒）
uint16_t count_1000_tick = 0;         // 1000ms 计数器

void sys_timer_init(void)
{
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 125); // 初始化定时器4,1MS一次中断
    TIM4_ARRPreloadConfig(ENABLE);              // 使能自动重装
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);      // 数据更新中断
    TIM4_Cmd(ENABLE);                           // 开定时器
}

void sys_timer_update_handler(void)
{
    // Check update interrupt flag
    if (TIM4_GetITStatus(TIM4_IT_UPDATE) != RESET)
    {
        // 10ms callback processing
        system_ticks++;
        system_time_ms++;
        count_1000_tick++;
        if (count_1000_tick == 1000)
        {
            count_1000_tick = 0;
            system_time_s++;
        }
        TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
    }
}

// 获取系统时间 (毫秒)
uint32_t sys_timer_get_system_time_ms(void)
{
    return system_time_ms;
}

// 获取系统时间 (秒)
uint32_t sys_timer_get_system_time_sec(void)
{
    return system_time_s;
}
