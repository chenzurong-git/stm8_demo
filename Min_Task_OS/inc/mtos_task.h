#ifndef __MTOS_TASK_H__
#define __MTOS_TASK_H__

#include "bsp_sys_pub.h"
#include "mtos_list.h"

typedef enum
{
    MTOS_TASK_STATUS_IDLE = 0,
    MTOS_TASK_STATUS_READY = 1,
    MTOS_TASK_STATUS_RUNNING = 2,
    MTOS_TASK_STATUS_SUSPEND = 3,
    MTOS_TASK_STATUS_STOPPED = 4,
} mtos_task_status_t;

typedef struct
{
    const char *name;        /* 任务名称 */
    void (*task_func)(void); /* 任务函数 */
    void (*pre_init)(void);  /* 任务前置初始化函数 */
} mtos_task_func_t;

/* 任务信息结构体 */
typedef struct
{
    mtos_task_func_t func;      /* 任务函数 */
    mtos_list_node_t list_node; /* 用于挂载到全局链表 */
    uint16_t time_period;       /* 任务周期 */
    uint32_t last_run_time;     /* 上次运行时间 */
    uint8_t run_now_flag;       /* 运行标志，设置为1时强制运行 */
    mtos_task_status_t status;  /* 任务状态 */
} mtos_task_t;

/* 全局任务链表 */
extern mtos_list_t mtos_task_list;

void mtos_init(void);

void mtos_task_create(char *name, void (*task)(void), void (*pre_init)(void), uint16_t time_period);

/**
 * @brief 删除指定任务
 * @param task 任务指针
 * @return 是否删除成功
 */
bool mtos_task_delete(mtos_task_t *task);

/**
 * @brief 根据任务名称立即执行任务
 * @param name 任务名称
 * @return 是否找到并执行了任务
 */
bool mtos_task_execute_by_name(const char *name);
void mtos_task_show(void);
void mtos_task_schedule(void);

#endif
