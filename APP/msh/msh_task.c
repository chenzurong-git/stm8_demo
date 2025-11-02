#include "bsp_sys_pub.h"
#include "msh_task.h"

extern const msh_cmd_t *__msh_cmd_start;
extern const msh_cmd_t *__msh_cmd_end;

const msh_cmd_t msh_list[] =
    {
        MSH_CMD_DEF(echo, "Echo input string", msh_cmd_echo),
        MSH_CMD_DEF(help, "List all commands", msh_cmd_help),
        MSH_CMD_DEF(clear, "Clear screen", msh_cmd_clear),
};

void msh_cmd_init()
{
    __msh_cmd_start = msh_list;
    __msh_cmd_end = __msh_cmd_start + sizeof(msh_list) / sizeof(msh_list[0]);
}

void msh_task_init(void)
{
    msh_init();
    mtos_task_create("msh_task", msh_process, msh_cmd_init, 10);
    uart_set_rx_callback(msh_rx_input);
}
