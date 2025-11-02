#ifndef __MSH_H__
#define __MSH_H__

#include "bsp_sys_pub.h"
// 配置参数
#define MSH_CMD_MAX_LENGTH     64    // 命令最大长度
#define MSH_ARG_MAX_COUNT      8     // 最大参数数量
#define MSH_HISTORY_MAX_COUNT  5     // 历史命令最大数量
#define MSH_UART_BUFFER_SIZE   128   // UART缓冲区大小
 typedef int (*mshfunc)(int argc, char **argv);
// 命令结构体
typedef struct {
    const char *name;            // 命令名称
    const char *desc;            // 命令描述
    mshfunc func;  // 命令处理函数
} msh_cmd_t;

// 命令注册宏
#define MSH_CMD_EXPORT(name, desc, func) \
    { \
        #name, desc, func \
    }
#define MSH_CMD_DEF(name, desc, func) \
    { \
        #name, desc, (mshfunc)func \
    }

//内置命令
int msh_cmd_echo(int argc, char **argv);
int msh_cmd_help(int argc, char **argv);
int msh_cmd_clear(int argc, char **argv);

// MSH终端初始化
void msh_init(void);

// MSH终端主处理函数（主循环中调用）
void msh_process(void);
// MSH接收中断回调函数（由硬件驱动调用）
void msh_rx_input(uint8_t data);
// 发送字符串助手函数
void uart_send_bytes(const uint8_t *data, uint16_t len);
static inline void msh_print(const char *str) {
    uart_send_bytes((const uint8_t*)str, strlen(str));
}
void msh_task_init(void);
#endif // MSH_H
