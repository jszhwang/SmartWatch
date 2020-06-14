/*
* file: app_main.h
* description: the major sreen after system waked up from sleep
*/

#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

#include "utilities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

typedef enum{
   APP_MAIN_STATE_IDLE = 0,
   APP_MAIN_STATE_RUNNING,
   APP_MAIN_STATE_TOTAL
}APP_MAIN_STATE;

typedef struct{
    APP_MAIN_STATE state;
    TaskHandle_t app_main_task;
    TimerHandle_t app_main_timer;
    SemaphoreHandle_t xButtonSemphor;
    uint8_t app_timer_counter;
}app_main_ctx_t;

void app_main_init(app_main_ctx_t *ctx);
#endif