#include "mtos_task.h"
#include <stddef.h>

mtos_list_t mtos_task_list; // 任务列表

/**
 * @brief 初始化任务列表
 */
void mtos_task_list_init(void)
{
    // 初始化任务列表
    mtos_list_init(&mtos_task_list);
}

/**
 * @brief 初始化任务系统
 */
void mtos_init(void)
{
    // 初始化任务列表
    mtos_task_list_init();
}

/**
 * @brief 创建任务
 * @param name 任务名称
 * @param task 任务函数
 * @param pre_init 任务前置初始化函数
 * @param time_period 任务执行周期
 */
void mtos_task_create(char *name, void (*task)(void), void (*pre_init)(void), uint16_t time_period)
{
    // 在嵌入式系统中，我们使用静态分配的方式创建任务
    // 实际应用中，可能需要使用内存池或动态分配
    static uint8_t task_memory[1024]; // 简单的静态内存缓冲区
    static size_t task_memory_used = 0;

    // 检查内存是否足够
    if (task_memory_used + sizeof(mtos_task_t) > sizeof(task_memory))
    {
        return; // 内存不足
    }

    // 分配任务内存
    mtos_task_t *new_task = (mtos_task_t *)(task_memory + task_memory_used);
    task_memory_used += sizeof(mtos_task_t);

    // 初始化任务信息
    new_task->func.name = name;
    new_task->func.task_func = task;
    new_task->func.pre_init = pre_init;
    new_task->time_period = time_period;
    new_task->last_run_time = 0;
    new_task->run_now_flag = 0;
    new_task->status = MTOS_TASK_STATUS_IDLE;

    // 初始化链表节点
    mtos_list_node_init(&new_task->list_node);

    // 将任务添加到链表尾部
    mtos_list_insert_tail(&mtos_task_list, &new_task->list_node);

    // 调用前置初始化函数（如果有）
    if (new_task->func.pre_init != NULL)
    {
        new_task->func.pre_init();
    }

    // 设置任务状态为就绪
    new_task->status = MTOS_TASK_STATUS_READY;
}

/**
 * @brief 删除指定任务
 * @param task 任务指针
 * @return 是否删除成功
 */
bool mtos_task_delete(mtos_task_t *task)
{
    if (task == NULL)
    {
        return FALSE;
    }

    // 将任务状态设置为停止
    task->status = MTOS_TASK_STATUS_STOPPED;

    // 从链表中移除任务节点
    mtos_list_node_t *removed = mtos_list_remove_node(&mtos_task_list, &task->list_node);

    // 清理任务节点
    if (removed != NULL)
    {
        // 重置链表节点
        mtos_list_node_init(&task->list_node);

        // 清空任务相关指针
        task->func.name = NULL;
        task->func.task_func = NULL;
        task->func.pre_init = NULL;

        return TRUE;
    }

    return FALSE;
}

/**
 * @brief 根据任务名称查找任务
 * @param name 任务名称
 * @return 找到的任务指针，如果未找到返回NULL
 */
static mtos_task_t *mtos_task_find_by_name(const char *name)
{
    mtos_list_node_t *node;

    // 遍历任务链表
    MTOS_LIST_FOR_EACH(&mtos_task_list, node)
    {
        // 获取任务结构体
        mtos_task_t *task = MTOS_LIST_ENTRY(node, mtos_task_t, list_node);

        // 比较任务名称
        if (task->func.name != NULL && name != NULL)
        {
            if (strcmp(task->func.name, name) == 0)
            {
                return task; // 找到匹配的任务
            }
        }
    }

    return NULL; // 未找到任务
}

/**
 * @brief 设置任务的运行标志
 * @param task 任务指针
 * @param flag 运行标志，1表示强制运行，0表示清除强制运行
 */
static void mtos_task_set_run_flag(mtos_task_t *task, uint8_t flag)
{
    if (task != NULL)
    {
        // 检查任务状态是否为就绪或运行中
        task->status = MTOS_TASK_STATUS_READY;
        task->run_now_flag = flag;
    }
}

/**
 * @brief 根据任务名称，下一个心跳执行任务
 * @param name 任务名称
 * @return 是否找到并执行了任务
 */
bool mtos_task_execute_by_name(const char *name)
{
    // 查找任务
    mtos_task_t *task = mtos_task_find_by_name(name);

    if (task == NULL || task->func.task_func == NULL)
    {
        return FALSE;
    }
    // 设置运行标志为1，确保下一个心跳执行
    mtos_task_set_run_flag(task, 1);
    return TRUE;
}

/**
 * @brief 显示所有任务信息
 */
void mtos_task_show(void)
{
    mtos_list_node_t *node;

    // 遍历任务链表
    MTOS_LIST_FOR_EACH(&mtos_task_list, node)
    {
        // 获取任务结构体
        mtos_task_t *task = MTOS_LIST_ENTRY(node, mtos_task_t, list_node);

        // 打印任务信息
        printf("MTOS Task Name: %s, Status: %d, Run Now Flag: %d, Last Run Time: %lu, Time Period: %d\r\n",
               task->func.name, task->status, task->run_now_flag, task->last_run_time, task->time_period);
    }
}

/**
 * @brief 任务调度器
 * @note 这个函数应该在定时中断中调用，或者在主循环中周期性调用
 */
void mtos_task_schedule(void)
{
    mtos_list_node_t *current = mtos_task_list.head;
    uint32_t current_time = sys_timer_get_system_time_ms();

    // 遍历任务链表（单向链表兼容）
    while (current != NULL)
    {
        // 保存下一个节点，防止任务执行过程中链表被修改
        mtos_list_node_t *next = current->next;

        // 获取任务结构体
        mtos_task_t *task = MTOS_LIST_ENTRY(current, mtos_task_t, list_node);

        // 检查任务状态
        if (task->status != MTOS_TASK_STATUS_READY && task->status != MTOS_TASK_STATUS_RUNNING)
        {
            current = next; // 移动到下一个任务
            continue;       // 跳过未就绪或已挂起的任务
        }
        // 计算任务运行时间，回绕处理
        uint32_t interval = (current_time >= task->last_run_time) ? (current_time - task->last_run_time) : (current_time + (0xFFFF - task->last_run_time));

        // 检查是否需要运行任务（周期到达或强制运行标志设置）
        if (interval >= task->time_period || task->run_now_flag)
        {
            task->run_now_flag = 0;
            // 运行任务
            if (task->func.task_func != NULL)
            {
                task->status = MTOS_TASK_STATUS_RUNNING;
                task->func.task_func();
                task->last_run_time = current_time;
                task->status = MTOS_TASK_STATUS_READY;
            }
        }
        // 移动到下一个任务
        current = next;
    }
}
