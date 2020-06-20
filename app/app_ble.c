#include "app_ble.h"
#include "nrf_log.h"



static uint16_t on_play_cmd(uint8_t *buf, uint16_t sz)
{
    //play music 
    buf[0] = 0x00;
    return 1;
}

static uint16_t on_pause_cmd(uint8_t *buf, uint16_t sz)
{
    buf[0] = 0x00;
    return 1;  
}

static uint16_t on_stop_cmd(uint8_t *buf, uint16_t sz)
{
    buf[0] = 0x00;
    return 1;
}


const CMD_TBL cmd_tbl[] = {
    {0x01, on_play_cmd},
    {0x02, on_pause_cmd},
    {0x03, on_stop_cmd},
};

void app_ble_task( void * parameter)
{
    BaseType_t ret; 
    APP_BLE *ctx = (APP_BLE *)parameter;
    while(1)
    {
        ret = xSemaphoreTake(ctx->app_ble_semaphore, portMAX_DELAY);
        if(ret == pdPASS)
        {
            ret = xSemaphoreTakeRecursive(ctx->app_ble_mutex, pdMS_TO_TICKS(10));
            if(ret == pdPASS)
            {
                do{
                    switch(ctx->app_ble_state)
                    {
                    case APP_BLE_STATE_SOP:
                        if(ctx->app_ble_pack->cmd_header.header == 0x55)
                        {
                            ctx->buf_tail++;
                            ctx->app_ble_state = APP_BLE_STATE_CMD;
                        }
                        break;
                    case APP_BLE_STATE_CMD:
                        {
                            ctx->buf_tail++;
                            for(uint8_t i = 0; i < sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
                            {
                                if(ctx->app_ble_pack->cmd_header.cmd == cmd_tbl[i].cmd)
                                {
                                    uint16_t len = cmd_tbl[i].fn(&ctx->app_ble_pack->cmd_buf[sizeof(ctx->app_ble_pack->cmd_header)], \
                                        ctx->app_ble_pack->cmd_header.length);
                                    ctx->app_ble_pack->cmd_header.length = len;
                                    ctx->cb(ctx->app_ble_pack->cmd_buf, len + sizeof(ctx->app_ble_pack->cmd_header));
                                }
                            }
                            ctx->app_ble_state = APP_BLE_STATE_SOP;
                        }
                        break;
                    case APP_BLE_STATE_LEN:
                    case APP_BLE_STATE_DAT:
                    default: 
                        ctx->app_ble_state = APP_BLE_STATE_SOP;
                        break;
                    }
                }while(ctx->app_ble_state != APP_BLE_STATE_SOP);
            }
            xSemaphoreGiveRecursive(ctx->app_ble_mutex);
        }
    }

}

void app_ble_init(APP_BLE *ctx)
{
    ctx->app_ble_semaphore = xSemaphoreCreateBinary();
    ctx->app_ble_mutex = xSemaphoreCreateRecursiveMutex();
    xTaskCreate(app_ble_task, "btask", 128, ctx, 5, &ctx->app_ble_task);
}


void app_ble_set_data(APP_BLE *ctx, uint8_t *buf, uint16_t sz)
{
    BaseType_t ret;
    ret = xSemaphoreTakeRecursive(ctx->app_ble_mutex, portMAX_DELAY);

    if(ret == pdPASS)
    {
        memcpy(ctx->app_ble_pack->cmd_buf, buf, sz);
        ctx->buf_head = sz;
        ctx->buf_tail = 0;
    }

    xSemaphoreGiveRecursive(ctx->app_ble_mutex);
}

void app_ble_set_data_notification(APP_BLE *ctx, bool notified)
{
    if(notified)
    {
        xSemaphoreGive(ctx->app_ble_semaphore);
    }
}