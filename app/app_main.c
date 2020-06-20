/*
* file: app_main.c
* description: the major sreen after system waked up from sleep
*/
#include "app_main.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

typedef void *(*fn)(app_main_ctx_t *ctx);

app_main_ctx_t *p_app_main_ctx;
void app_timer_event(TimerHandle_t xTimer)
{
    uint8_t item = 0x5A;
    xQueueSend(p_app_main_ctx->btn_msg, &item, 0);
}

void *app_main_screen(app_main_ctx_t *ctx)
{
    uint8_t sys_code;
    while(1)
    {
        sys_code = 0;
        xQueueReceive(ctx->btn_msg, &sys_code, portMAX_DELAY);
        if(sys_code == 0x5A)    /* !<Hanlde timer event*/
        {
            ctx->app_timer_counter++;
        }
        else                     /* !<Hanlde key event*/
        {
            NRF_LOG_INFO("Key code is %d", sys_code);
        }
        
    } 
}

void *app_setup_screen(app_main_ctx_t *ctx)
{
    while(1)
    {
    }
}

void app_main_func(void *arg)
{
    app_main_ctx_t *pCtx = (app_main_ctx_t *)arg;
    fn pfn = NULL;
    uint8_t key_code;
    while(1)
    {
        switch (pCtx->state)
        {
        case APP_MAIN_STATE_IDLE:
            xQueueReceive(pCtx->btn_msg, &key_code, portMAX_DELAY);
            pCtx->state = APP_MAIN_STATE_RUNNING;
            pfn = app_main_screen;
            break;
        case APP_MAIN_STATE_RUNNING:
            xTimerStart(pCtx->app_main_timer, 0);
            do{
                pfn = (fn)pfn(pCtx);
            }while(pfn != NULL);
            xTimerStop(pCtx->app_main_timer,0);
            pCtx->state = APP_MAIN_STATE_IDLE;
            break;
        default:
            break;
        }
    }
}



void app_main_init(app_main_ctx_t *ctx)
{ 
    p_app_main_ctx = ctx;
    ctx->state = APP_MAIN_STATE_IDLE;
    if(pdFAIL == xTaskCreate(app_main_func, "man", 128, (void*)ctx, 1, &ctx->app_main_task ))
    {
        NRF_LOG_ERROR("Create task man failed");
    }
    ctx->xButtonSemphor = xSemaphoreCreateBinary();
    ctx->btn_msg = xQueueCreate(5, 1);
    ctx->app_main_timer = xTimerCreate("aptmr", pdMS_TO_TICKS(1000), pdTRUE, NULL, app_timer_event);
   
}