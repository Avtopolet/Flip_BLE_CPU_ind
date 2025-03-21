#ifndef BLE_CUSTOM_SERVICE_H
#define BLE_CUSTOM_SERVICE_H
#pragma once



#include <furi.h>
#include <ble/ble.h>
#include <furi_ble/event_dispatcher.h>
#include <lib/stm32wb_copro/wpan/ble/core/auto/ble_gatt_aci.h>
#include <furi_ble/gatt.h>
#include "ble_reader.h"
//#include <ble/ble.h>

#include <lib/stm32wb_copro/wpan/interface/patterns/ble_thread/tl/hci_tl.h>

typedef struct {
    uint16_t svc_handle;       // Хэндл сервиса
    uint16_t char_handle;      // Хэндл характеристики
    uint16_t conn_handle;      // Хэндл соединения
    void* attr_value; 
    void* event_handler;       // Обработчик событий
    uint8_t buffer[128]; // Буфер для хранения прочитанных данных
    uint16_t data_length; // Длина прочитанных данных
    bool data_ready; // Флаг, указывающий, что данные готовы
    //BleEventDispatcherSvcHandler* event_handler; // Обработчик событий
    BleReaderApp* app; // Указатель на приложение

} BleCustomService;

//#ifdef __cplusplus
//extern "C" {
//#endif
//BleCustomService* ble_custom_service_start(void);
BleCustomService* ble_custom_service_start(void* ctx);
bool ble_custom_service_update_value(BleCustomService* svc, const uint8_t* data, uint16_t len);
bool ble_custom_service_get_read_data(BleCustomService* svc, uint8_t* buffer, uint16_t* len);
bool ble_custom_service_read_value(BleCustomService* svc);
void ble_custom_service_stop(BleCustomService* svc);
//tBleStatus ble_custom_service_add_char(BleCustomService* svc);
//#ifdef __cplusplus
//}
//#endif

#endif