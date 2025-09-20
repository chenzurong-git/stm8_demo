#include "bsp_sys_pub.h"

void bsp_sys_init(void)
{
    bsp_clk_config_init();    // 时钟配置初始化
    uart_hw_init(115200); // UART1初始化
}

void assert_failed(uint8_t *file, uint32_t line) {
    // 输出断言失败信息到串口（假设已初始化UART）
    printf("Assert failed: file %s, line %lu\r\n", file, line);

    // 断言失败后进入死循环，防止程序继续运行
    while (1) {
        // 可以添加LED闪烁等错误指示
    }
}
