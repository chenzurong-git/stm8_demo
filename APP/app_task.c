#include "app_task.h"

void task1(void)
{
    printf("task1 running!\r\n");
}

void task2(void)
{
    printf("task2 running!\r\n");
}

void app_task_init(void)
{
    // mtos_task_create("task1", task1, NULL, 1000);
    // mtos_task_create("task2", task2, NULL, 1000);
}
