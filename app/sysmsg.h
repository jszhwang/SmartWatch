#ifndef __SYS_MSG_H__
#define __SYS_MSG_H__

#include <bool.h>
#include "queue.h"
#include "timers.h"

#define SYS_MSG_BTN   0
#define SYS_MSG_TICK  1

#deifne SYS_MSG_BTN_HOLD     0x8000
#define SYS_MSG_BTN_RELEASE  0x4000
#define SYS_MSG_BTN_CLICK    0x2000

typedef struct {
    uint16_t type;
    uint16_t msg;
}system_msg_t;

typedef struct {
    system_msg_t msg;
    TimerHandle_t sys_tick;
    QueueHandle_t sys_msg_mb;
}sysmsg_t;

void sysmsg_init(sysmsg_t *sysmsg);
bool sysmsg_post(system_msg_t msg);
bool sysmsg_retrive(system_msg_t *const msg);


#endif