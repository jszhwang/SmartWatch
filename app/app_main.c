/*
* file: app_main.c
* description: the major sreen after system waked up from sleep
*/
#include "app_main.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

typedef void *(*fn)(app_main_ctx_t *ctx);

void *app_main_screen(app_main_ctx_t *ctx)
{
    while(1)
    {
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
    while(1)
    {
        switch (pCtx->state)
        {
        case APP_MAIN_STATE_IDLE:
            xSemaphoreTake(pCtx->xButtonSemphor,portMAX_DELAY); 
            pCtx->state = APP_MAIN_STATE_RUNNING;
            pfn = app_main_screen;
            NRF_LOG_INFO("Key pressed");
            break;
        case APP_MAIN_STATE_RUNNING:
            do{
                pfn = (fn)pfn(pCtx);
            }while(pfn != NULL);
            pCtx->state = APP_MAIN_STATE_IDLE;
            break;
        default:
            break;
        }
    }
}



void app_main_init(app_main_ctx_t *ctx)
{
    ctx->state = APP_MAIN_STATE_IDLE;
    if(pdFAIL == xTaskCreate(app_main_func, "man", 128, (void*)ctx, 1, &ctx->app_main_task ))
    {
        NRF_LOG_ERROR("Create task man failed");
    }
    ctx->xButtonSemphor = xSemaphoreCreateBinary();
}