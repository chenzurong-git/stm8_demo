#include "bsp_sys_pub.h"
int main(void)
{
    bsp_sys_init();
    while (1)
    {
        // uart_send_bytes("hello world!\r\n", strlen("hello world!\r\n"));
        printf("hello world!\r\n");
        delay_ms(1000);
    }
}
