#include "bsp_sys_pub.h"
#include "mtos_task.h"
#include "app_task.h"
#include "msh_task.h"

int main(void)
{
	bsp_sys_init();
	mtos_init();
	msh_task_init();
	app_task_init();
	mtos_task_show();
	while (1)
	{
		mtos_task_schedule();
	}
}
