#include "sysmsg.h"

void sysmsg_init(sysmsg_t *sysmsg)
{
    sysmsg->sys_msg_mb = xQueueCreate(1, sizeof(sysmsg_t));
    sysmsg->sys_tick = xTimerCreate("stmr", pdMS_TO_TICK(100), pdTURE, NULL, systmr_event_handler);
}