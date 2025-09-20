#include "bsp_clk.h"

void bsp_clk_config_init(void)
{
    // 重置时钟配置
    CLK_DeInit();

    // 开启外部高速振荡器（HSE）24MHz
    CLK_HSECmd(ENABLE);

    // 等待HSE稳定
    while (CLK_GetFlagStatus(CLK_FLAG_HSERDY) == RESET)
        ;

    // 配置系统时钟源为HSE，使用手动切换模式
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_MANUAL, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_DISABLE);

    // 配置系统时钟分频器为1分频（24MHz）
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

    // 配置HSI分频器为1分频
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

    // 配置外设时钟
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE); // Timer2
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);  // UART1
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);    // SPI
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);    // I2C
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);    // ADC

    // 打开总中断
    rim();
}
