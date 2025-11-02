#ifndef SYS_TIMER_H
#define SYS_TIMER_H

void sys_timer_init(void);
void sys_timer_update_handler(void);
uint32_t sys_timer_get_tick_ms(void);
uint32_t sys_timer_get_system_time_sec(void);
uint32_t sys_timer_get_system_time_ms(void);

#endif
