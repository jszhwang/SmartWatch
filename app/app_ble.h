#ifndef __APP_BLE_H__
#define __APP_BLE_H__
#include <stdint.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

typedef uint16_t(*app_ble_event_handler_t)(uint8_t *buf, uint16_t sz);

typedef struct {
    uint8_t cmd;
    app_ble_event_handler_t fn;
}CMD_TBL;

enum app_ble_state{
    APP_BLE_STATE_SOP = 0,
    APP_BLE_STATE_CMD,
    APP_BLE_STATE_LEN,
    APP_BLE_STATE_DAT,
    APP_BLE_STATE_INVALID
};

#pragma pack(push, 1)
typedef struct app_ble_proto_header{
    uint8_t header;
    uint8_t cmd;
    uint8_t length;
}APP_BLE_PROTO_HEADER;
#pragma pack(pop)

union app_ble_proto{
    APP_BLE_PROTO_HEADER cmd_header;
    uint8_t cmd_buf[128];
};
    

typedef struct app_ble
{
    app_ble_event_handler_t cb;
    enum app_ble_state app_ble_state;
    union app_ble_proto app_ble_packet;
    int32_t buf_sz;
    int32_t buf_head;
    int32_t buf_tail;
    SemaphoreHandle_t app_ble_semaphore;
    SemaphoreHandle_t app_ble_mutex;
    TaskHandle_t app_ble_task;
}APP_BLE;


void app_ble_init(app_ble_event_handler_t cb);
void app_ble_set_data(uint8_t *buf, uint16_t sz);
void app_ble_set_data_notification(bool notified);

#endif