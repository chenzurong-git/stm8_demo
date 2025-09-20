#include "bsp_uart.h"
#include "stm8s_uart1.h"

// UART硬件初始化
void uart_hw_init(u32 baudrate)
{
    //配置串口参数为：波特率115200，8位数据位，1位停止位，无校验，禁止同步传输，允许接收发送
    UART1_Init(baudrate,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE);
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);//使能接收中断
    UART1_Cmd(ENABLE);//使能UART1
}

// UART发送字节
void uart_send_byte(uint8_t data) {
    UART1_SendData8(data);//UART1发送8位数据
    while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);//等待发送完成
}

// UART发送字节数组
void uart_send_bytes(const uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        uart_send_byte(data[i]);
    }
}

// UART1接收中断服务程序
INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18) {
    // 检查接收中断标志
    if (UART1_GetITStatus(UART1_IT_RXNE) != RESET) {
        // 读取接收的数据
        uint8_t data = UART1_ReceiveData8();
        // 清除中断标志
        UART1_ClearITPendingBit(UART1_IT_RXNE);
    }
}
