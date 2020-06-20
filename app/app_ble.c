#include "app_ble.h"
#include "nrf_log.h"

static APP_BLE app_ble_ctx = {0};


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
    while(1)
    {
        ret = xSemaphoreTake(app_ble_ctx.app_ble_semaphore, portMAX_DELAY);
        if(ret == pdPASS)
        {
            ret = xSemaphoreTakeRecursive(app_ble_ctx.app_ble_mutex, pdMS_TO_TICKS(10));
            if(ret == pdPASS)
            {
                do{
                    switch(app_ble_ctx.app_ble_state)
                    {
                    case APP_BLE_STATE_SOP:
                        if(app_ble_ctx.app_ble_packet.cmd_header.header == 0x55)
                        {
                            app_ble_ctx.buf_tail++;
                            app_ble_ctx.app_ble_state = APP_BLE_STATE_CMD;
                        }
                        break;
                    case APP_BLE_STATE_CMD:
                        {
                            app_ble_ctx.buf_tail++;
                            for(uint8_t i = 0; i < sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
                            {
                                if(app_ble_ctx.app_ble_packet.cmd_header.cmd == cmd_tbl[i].cmd)
                                {
                                    uint16_t len = cmd_tbl[i].fn(&app_ble_ctx.app_ble_packet.cmd_buf[sizeof(app_ble_ctx.app_ble_packet.cmd_header)], \
                                        app_ble_ctx.app_ble_packet.cmd_header.length);
                                    app_ble_ctx.app_ble_packet.cmd_header.length = len;
                                    app_ble_ctx.cb(app_ble_ctx.app_ble_packet.cmd_buf, len + sizeof(app_ble_ctx.app_ble_packet.cmd_header));
                                }
                            }
                            app_ble_ctx.app_ble_state = APP_BLE_STATE_SOP;
                        }
                        break;
                    case APP_BLE_STATE_LEN:
                    case APP_BLE_STATE_DAT:
                    default: 
                        app_ble_ctx.app_ble_state = APP_BLE_STATE_SOP;
                        break;
                    }
                }while(app_ble_ctx.app_ble_state != APP_BLE_STATE_SOP);
            }
            xSemaphoreGiveRecursive(app_ble_ctx.app_ble_mutex);
        }
    }

}

void app_ble_init(app_ble_event_handler_t cb)
{
    app_ble_ctx.app_ble_semaphore = xSemaphoreCreateBinary();
    app_ble_ctx.app_ble_mutex = xSemaphoreCreateRecursiveMutex();
    xTaskCreate(app_ble_task, "btask", 128, NULL, 5, &app_ble_ctx.app_ble_task);
    if(cb != NULL)
    {
        app_ble_ctx.cb = cb;
    }
}

void app_ble_set_data(uint8_t *buf, uint16_t sz)
{
    BaseType_t ret;
    ret = xSemaphoreTakeRecursive(app_ble_ctx.app_ble_mutex, portMAX_DELAY);

    if(ret == pdPASS)
    {
        memcpy(app_ble_ctx.app_ble_packet.cmd_buf, buf, sz);
        app_ble_ctx.buf_head = sz;
        app_ble_ctx.buf_tail = 0;
    }

    xSemaphoreGiveRecursive(app_ble_ctx.app_ble_mutex);
}

void app_ble_set_data_notification(bool notified)
{
    if(notified)
    {
        xSemaphoreGive(app_ble_ctx.app_ble_semaphore);
    }
}