
#include "msh.h"
#include "stm8s.h"

// 外部命令段边界（由链接脚本定义）
const msh_cmd_t *__msh_cmd_start;
const msh_cmd_t *__msh_cmd_end;

// MSH终端状态数据
static struct
{
    char cmd_buf[MSH_CMD_MAX_LENGTH]; // 命令缓冲区
    uint16_t cmd_len;                 // 当前命令长度
    char *argv[MSH_ARG_MAX_COUNT];    // 参数列表
    int argc;                         // 参数数量

    // 历史命令相关
    char history[MSH_HISTORY_MAX_COUNT][MSH_CMD_MAX_LENGTH];
    uint8_t history_count; // 历史命令数量
    int8_t history_index;  // 当前历史命令索引

    // 接收缓冲区相关
    uint8_t recv_buf[MSH_UART_BUFFER_SIZE];
    uint16_t recv_head;
    uint16_t recv_tail;
} msh_data = {0};

// 声明内部函数
static void msh_parse_command(void);
static void msh_execute_command(void);
static void msh_print_prompt(void);
static void msh_add_history(const char *cmd);
static void msh_handle_special_key(uint8_t c);
static void msh_complete_command(void);
static uint16_t msh_get_recv_count(void);
static uint8_t msh_read_char(void);

// 内置echo命令
int msh_cmd_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
    printf("\r\n");
    return 0;
}

// 内置help命令
int msh_cmd_help(int argc, char **argv)
{
    const msh_cmd_t *cmd;

    printf("Available commands:\r\n");
    for (cmd = __msh_cmd_start; cmd < __msh_cmd_end; cmd++)
    {
        if (cmd->name && cmd->func)
        {
            printf("  %-10s - %s\r\n", cmd->name, cmd->desc);
        }
    }
    return 0;
}

// 内置clear命令
int msh_cmd_clear(int argc, char **argv)
{
    // 发送清屏ESC序列
    printf("\033[2J\033[1;1H");
    return 0;
}

// 获取接收缓冲区数据量
static uint16_t msh_get_recv_count(void)
{
    uint16_t count = 0;
    if (msh_data.recv_head >= msh_data.recv_tail)
    {
        count = msh_data.recv_head - msh_data.recv_tail;
    }
    else
    {
        count = MSH_UART_BUFFER_SIZE - msh_data.recv_tail + msh_data.recv_head;
    }
    return count;
}

// 从接收缓冲区读取一个字符
static uint8_t msh_read_char(void)
{
    uint8_t c = msh_data.recv_buf[msh_data.recv_tail];
    msh_data.recv_tail = (msh_data.recv_tail + 1) % MSH_UART_BUFFER_SIZE;
    return c;
}

// MSH终端初始化
void msh_init(void)
{
    // 初始化缓冲区和变量
    msh_data.cmd_len = 0;
    msh_data.argc = 0;
    msh_data.history_count = 0;
    msh_data.history_index = -1;
    msh_data.recv_head = 0;
    msh_data.recv_tail = 0;
    memset(msh_data.cmd_buf, 0, MSH_CMD_MAX_LENGTH);
    memset(msh_data.argv, 0, sizeof(msh_data.argv));
    memset(msh_data.history, 0, sizeof(msh_data.history));

    // 打印欢迎信息
    printf("Welcome to MSH Terminal!!\r\n");
    printf("Build: %s\r\n", __DATE__);
    msh_print_prompt();
}

// 打印命令提示符
static void msh_print_prompt(void)
{
    printf("msh> ");
}

// 解析命令行
static void msh_parse_command(void)
{
    msh_data.argc = 0;
    memset(msh_data.argv, 0, sizeof(msh_data.argv));

    // 跳过前导空格
    uint16_t i = 0;
    while (i < msh_data.cmd_len && msh_data.cmd_buf[i] == ' ')
    {
        i++;
    }

    // 解析参数
    while (i < msh_data.cmd_len && msh_data.argc < MSH_ARG_MAX_COUNT)
    {
        msh_data.argv[msh_data.argc++] = &msh_data.cmd_buf[i];

        // 找到参数结束（空格或结束）
        while (i < msh_data.cmd_len && msh_data.cmd_buf[i] != ' ')
        {
            i++;
        }

        // 替换空格为结束符
        if (i < msh_data.cmd_len)
        {
            msh_data.cmd_buf[i++] = '\0';
        }
    }
}

// 执行命令
static void msh_execute_command(void)
{
    if (msh_data.cmd_len == 0)
    {
        return;
    }

    // 添加到历史记录
    msh_add_history(msh_data.cmd_buf);

    // 解析命令
    msh_parse_command();

    // 查找并执行命令
    const msh_cmd_t *cmd;
    int found = 0;

    for (cmd = __msh_cmd_start; cmd < __msh_cmd_end; cmd++)
    {
        if (cmd->name && strcmp(msh_data.argv[0], cmd->name) == 0)
        {
            cmd->func(msh_data.argc, msh_data.argv);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Command not found: %s\r\n", msh_data.argv[0]);
    }
}

// 添加命令到历史记录
static void msh_add_history(const char *cmd)
{
    // 忽略空命令和与上一条相同的命令
    if (strlen(cmd) == 0)
    {
        return;
    }
    if (msh_data.history_count > 0 &&
        strcmp(cmd, msh_data.history[msh_data.history_count - 1]) == 0)
    {
        return;
    }

    // 如果历史记录已满，移除最旧的一条
    if (msh_data.history_count >= MSH_HISTORY_MAX_COUNT)
    {
        for (int i = 0; i < MSH_HISTORY_MAX_COUNT - 1; i++)
        {
            strcpy(msh_data.history[i], msh_data.history[i + 1]);
        }
        msh_data.history_count--;
    }

    // 添加新命令
    strncpy(msh_data.history[msh_data.history_count], cmd, MSH_CMD_MAX_LENGTH - 1);
    msh_data.history[msh_data.history_count][MSH_CMD_MAX_LENGTH - 1] = '\0';
    msh_data.history_count++;

    // 重置历史索引
    msh_data.history_index = -1;
}

// 处理特殊按键（退格、上下箭头、Tab）
static void msh_handle_special_key(uint8_t c)
{
    // 退格键 (ASCII 8)
    if (c == 8 || c == 127)
    { // 处理Backspace和Delete
        if (msh_data.cmd_len > 0)
        {
            msh_data.cmd_len--;
            msh_data.cmd_buf[msh_data.cmd_len] = '\0';

            // 发送退格序列：清除屏幕上的字符
            printf("\b \b");
        }
        return;
    }

    // 处理方向键（ESC序列）
    if (c == 0x1B)
    { // ESC字符
        if (msh_get_recv_count() >= 2)
        {
            uint8_t next1 = msh_read_char();
            uint8_t next2 = msh_read_char();

            if (next1 == '[')
            {
                // 上箭头
                if (next2 == 'A')
                {
                    if (msh_data.history_count > 0)
                    {
                        // 如果是第一次按上键，保存当前输入
                        if (msh_data.history_index == -1)
                        {
                            msh_data.history_index = msh_data.history_count - 1;
                        }
                        else if (msh_data.history_index > 0)
                        {
                            msh_data.history_index--;
                        }

                        // 清除当前命令行
                        while (msh_data.cmd_len > 0)
                        {
                            msh_data.cmd_len--;
                            printf("\b \b");
                        }

                        // 显示历史命令
                        strcpy(msh_data.cmd_buf, msh_data.history[msh_data.history_index]);
                        msh_data.cmd_len = strlen(msh_data.cmd_buf);
                        printf("%s", msh_data.cmd_buf);
                    }
                }
                // 下箭头
                else if (next2 == 'B')
                {
                    if (msh_data.history_index != -1)
                    {
                        msh_data.history_index++;

                        // 清除当前命令行
                        while (msh_data.cmd_len > 0)
                        {
                            msh_data.cmd_len--;
                            printf("\b \b");
                        }

                        // 如果索引超出范围，显示空
                        if (msh_data.history_index >= msh_data.history_count)
                        {
                            msh_data.cmd_len = 0;
                            msh_data.history_index = -1;
                        }
                        else
                        {
                            // 显示历史命令
                            strcpy(msh_data.cmd_buf, msh_data.history[msh_data.history_index]);
                            msh_data.cmd_len = strlen(msh_data.cmd_buf);
                            printf("%s", msh_data.cmd_buf);
                        }
                    }
                }
            }
        }
        return;
    }

    // Tab键补全
    if (c == 9)
    {
        msh_complete_command();
        return;
    }
}

// 命令补全功能
static void msh_complete_command(void)
{
    if (msh_data.cmd_len == 0)
    {
        // 无输入时列出所有命令

        printf("\r\n");
        msh_cmd_help(0, NULL);
        printf("\r\n");
        msh_print_prompt();
        printf("%s", msh_data.cmd_buf);
        return;
    }

    // 查找匹配的命令
    const msh_cmd_t *cmd;
    const msh_cmd_t *match_cmd = NULL;
    int match_count = 0;
    char partial[MSH_CMD_MAX_LENGTH];

    strncpy(partial, msh_data.cmd_buf, msh_data.cmd_len);
    partial[msh_data.cmd_len] = '\0';

    // 统计匹配数量并记录第一个匹配项
    for (cmd = __msh_cmd_start; cmd < __msh_cmd_end; cmd++)
    {
        if (cmd->name && strncmp(cmd->name, partial, msh_data.cmd_len) == 0)
        {
            match_count++;
            if (match_cmd == NULL)
            {
                match_cmd = cmd;
            }
        }
    }

    if (match_count == 1 && match_cmd != NULL)
    {
        // 唯一匹配项，补全命令
        const char *cmd_name = match_cmd->name;
        int add_len = strlen(cmd_name) - msh_data.cmd_len;

        // 添加补全的字符
        strcpy(&msh_data.cmd_buf[msh_data.cmd_len], &cmd_name[msh_data.cmd_len]);
        msh_data.cmd_len += add_len;

        // 输出补全的部分并添加空格
        printf("%s ", &cmd_name[msh_data.cmd_len - add_len]);
        msh_data.cmd_len++; // 算上添加的空格
        msh_data.cmd_buf[msh_data.cmd_len - 1] = ' ';
    }
    else if (match_count > 1)
    {
        // 多个匹配项，显示所有匹配命令
        printf("\r\n");
        for (cmd = __msh_cmd_start; cmd < __msh_cmd_end; cmd++)
        {
            if (cmd->name && strncmp(cmd->name, partial, msh_data.cmd_len) == 0)
            {
                printf("%s  ", cmd->name);
            }
        }
        printf("\r\n");
        msh_print_prompt();
        printf("%s", msh_data.cmd_buf);
    }
}

// 处理接收到的字符
static void msh_handle_char(uint8_t c)
{
    // 处理特殊控制字符
    if (c < 0x20)
    {
        // 回车换行
        if (c == '\r' || c == '\n')
        {
            if (msh_data.cmd_len > 0)
            {
                printf("\r\n");
                msh_execute_command();

                // 重置命令缓冲区
                msh_data.cmd_len = 0;
                memset(msh_data.cmd_buf, 0, MSH_CMD_MAX_LENGTH);
            }
            else
            {
                printf("\r\n");
            }
            msh_print_prompt();
        }
        else
        {
            // 处理其他特殊键
            msh_handle_special_key(c);
        }
    }
    else
    {
        // 普通字符，添加到命令缓冲区
        if (msh_data.cmd_len < MSH_CMD_MAX_LENGTH - 1)
        {
            msh_data.cmd_buf[msh_data.cmd_len++] = c;
            msh_data.cmd_buf[msh_data.cmd_len] = '\0';
            uart_send_byte(c); // 回显字符
        }
    }
}

// MSH终端主处理函数
void msh_process(void)
{
    // 处理接收缓冲区中的所有字符
    while (msh_get_recv_count() > 0)
    {
        uint8_t c = msh_read_char();
        msh_handle_char(c);
    }
}

// MSH接收中断回调函数（由硬件驱动调用）
void msh_rx_input(uint8_t data)
{
    // 将接收到的数据放入缓冲区
    uint16_t next_head = (msh_data.recv_head + 1) % MSH_UART_BUFFER_SIZE;
    if (next_head != msh_data.recv_tail)
    { // 缓冲区未满
        msh_data.recv_buf[msh_data.recv_head] = data;
        msh_data.recv_head = next_head;
    }
}
