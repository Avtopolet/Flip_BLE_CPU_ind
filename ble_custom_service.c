#include "ble_custom_service.h"

#define TAG "BleCustomSvc"

static const uint16_t ble_svc_custom_uuid = 0x1234; // Ваш UUID

/*

static bool char_write_callback(const void* context, const uint8_t** data, uint16_t* data_len) {
    FURI_LOG_I(TAG, "Write callback: context=%p, data=%p, data_len=%p", context, data, data_len);

    // Проверка параметров
    if (!context || !data || !data_len) {
        FURI_LOG_E(TAG, "Invalid parameters in char_write_callback");
        return false;
    }

    // Пример обработки данных
    BleReaderApp* app = (BleReaderApp*)context;
    if (*data && *data_len > 0) {
        memcpy(app->char_data, *data, *data_len);
        app->char_data_len = *data_len;

        // Обновляем интерфейс
        if (app->view_port) {
            view_port_update(app->view_port);
        } else {
            FURI_LOG_E(TAG, "View port is not initialized");
        }
    }

    return true; // Успешная обработка
}
*/
// Callback для чтения данных



static bool char_read_callback(const void* context, const uint8_t** data, uint16_t* data_len) {
    //UNUSED(context);
    // Пример статических данных
    
    
    
    FURI_LOG_I(TAG, "Entering char_read_callback");
          // Если context не обязателен, уберите проверку
    if (!context) {
        FURI_LOG_W(TAG, "Context is NULL, but continuing...");
    }
        // Проверка обязательных параметров
        if (!data_len) {
            FURI_LOG_E(TAG, "data_len is NULL");
            return false;
        }
    
        // Если data == NULL, это запрос на получение длины данных
        static uint8_t dummy_data[] = {0x01, 0x02, 0x03, 0x03};
        
        if (data) {
            *data = dummy_data;
        }
    
        *data_len = sizeof(dummy_data);
        FURI_LOG_I(TAG, "Read callback: data=%p, data_len=%d", data ? *data : NULL, *data_len);
        return false;
}

// Обработчик событий BLE
static BleEventAckStatus ble_custom_service_event_handler(void* event, void* context) {
    BleCustomService* svc = (BleCustomService*)context;
    if (!svc) {
        FURI_LOG_E(TAG, "Invalid context");
        return BleEventNotAck;
    }
    
    FURI_LOG_I(TAG, " >>>>----------------------- BLE event handler called-----------------------<<<");

    //uint8_t* event_data = (uint8_t*)event;
    //uint16_t event_code = *(uint16_t*)event_data;
    //FURI_LOG_I(TAG, "Event code: 0x%04X", event_code); // Добавьте этот лог
    BleEventAckStatus ret = BleEventNotAck;

    hci_uart_pckt* hci_pckt = (hci_uart_pckt*)event;
    hci_event_pckt* event_pckt = (hci_event_pckt*)hci_pckt->data;

    if (event_pckt->evt == HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE) {
        evt_blecore_aci* blecore_evt = (evt_blecore_aci*)event_pckt->data;
        uint16_t vs_evt_code = blecore_evt->ecode;
        FURI_LOG_I(TAG, "VS Event code: 0x%04X", vs_evt_code);

        switch (vs_evt_code) {
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE: {
            aci_gatt_attribute_modified_event_rp0* attribute_modified = 
                (aci_gatt_attribute_modified_event_rp0*)blecore_evt->data;
            FURI_LOG_I(TAG, " <<<< Attribute handle: %d", attribute_modified->Attr_Handle);
            FURI_LOG_I(TAG, "<<< Characteristic handle: %d", svc->char_handle);

            FURI_LOG_I(TAG, "Attribute handle: %d", attribute_modified->Attr_Handle);
            
           // if (attribute_modified->Attr_Handle == svc->char_handle) {

                FURI_LOG_I(TAG, "Data length: %d", attribute_modified->Attr_Data_Length);
                FURI_LOG_I(TAG, "Data: %02X %02X %02X", 
                           attribute_modified->Attr_Data[0], 
                           attribute_modified->Attr_Data[1], 
                           attribute_modified->Attr_Data[2]);
                
                
                
                
                memcpy(svc->buffer, attribute_modified->Attr_Data, attribute_modified->Attr_Data_Length);
                svc->data_length = attribute_modified->Attr_Data_Length;
                svc->data_ready = true;
                FURI_LOG_I(TAG, "Attribute modified: %d bytes", svc->data_length);
                // Копируем данные в приложение
                if (svc->app) {
                    memcpy(svc->app->char_data, svc->buffer, svc->data_length);
                    svc->app->char_data_len = svc->data_length;
                    // Извлекаем загрузку процессора из данных
                    svc->app->cpu_percent = (attribute_modified->Attr_Data[1] << 8) | attribute_modified->Attr_Data[2];
                    FURI_LOG_I(TAG, "CPU Load: %d%%", svc->app->cpu_percent);


                    svc->app->cpu_history[svc->app->history_index] = svc->app->cpu_percent;
                    svc->app->history_index = (svc->app->history_index + 1) % HISTORY_SIZE;

                    //svc->app->data_ready = true;

                    // Обновляем интерфейс
                    if (svc->app->view_port) {
                        view_port_update(svc->app->view_port);
                    } else {
                        FURI_LOG_E(TAG, "View port is not initialized");
                    }}
                    


                ret = BleEventAckFlowEnable;
           // }
            break;
        }
        case ACI_ATT_READ_RESP_VSEVT_CODE: {
            aci_att_read_resp_event_rp0* read_resp = 
                (aci_att_read_resp_event_rp0*)blecore_evt->data;

            svc->data_length = read_resp->Event_Data_Length;
            memcpy(svc->buffer, read_resp->Attribute_Value, svc->data_length);
            svc->data_ready = true;
            FURI_LOG_I(TAG, "Read response: %d bytes", svc->data_length);
            ret = BleEventAckFlowEnable;
            break;
        }
        default:
            break;
        }
    }

    return ret;
}
/*
static bool
    dev_info_char_data_callback(const void* context, const uint8_t** data, uint16_t* data_len) {
    *data_len = (uint16_t)strlen(context); //-V1029
    if(data) {
        *data = (const uint8_t*)context;
    }
    return false;
}*/


// Создание сервиса
BleCustomService* ble_custom_service_start(void* ctx) {

    if (!ctx) {
        FURI_LOG_E(TAG, "Context is NULL");
        return NULL;
    } else {FURI_LOG_I(TAG, "Context not NULL");};
   
    
    FURI_LOG_I(TAG, "Begin of ble_custom_service_start");

    // Проверка размера структуры
    FURI_LOG_I(TAG, "Size of BleReaderApp: %zu", sizeof(BleReaderApp));

    // Вывод содержимого ctx
    uint8_t* ctx_bytes = (uint8_t*)ctx;
    for (size_t i = 0; i < sizeof(BleReaderApp); i++) {
        FURI_LOG_I(TAG, "ctx[%zu] = 0x%02X", i, ctx_bytes[i]);
    };




   
    BleReaderApp* app = (BleReaderApp*)ctx;
    FURI_LOG_I(TAG, "Context transmitted: %p", app);
          // Проверка полей структуры
          FURI_LOG_I(TAG, "app->event_queue: %p", app->event_queue);
          FURI_LOG_I(TAG, "app->view_port: %p", app->view_port);
          FURI_LOG_I(TAG, "app->char_handle: %d", app->char_handle);
          FURI_LOG_I(TAG, "app->char_data_len: %d", app->char_data_len);

    
   // Выделение памяти для сервиса
   BleCustomService* custom_svc = malloc(sizeof(BleCustomService));
   if (!custom_svc) {
       FURI_LOG_E(TAG, "Failed to allocate memory for BleCustomService");
       return NULL;
   }
   FURI_LOG_I(TAG, "Memory allocated for BleCustomService: %p", custom_svc);
   custom_svc->app = app; // Сохраняем указатель на приложение

   // Регистрация обработчика событий
   custom_svc->event_handler =
       ble_event_dispatcher_register_svc_handler(ble_custom_service_event_handler, custom_svc);
   if (!custom_svc->event_handler) {
       FURI_LOG_E(TAG, "Failed to register event handler");
       free(custom_svc);
       return NULL;
   }
   FURI_LOG_I(TAG, "Event handler registered: %p", custom_svc->event_handler);

   // Добавление сервиса
   if (!ble_gatt_service_add(
           UUID_TYPE_16,
           (const void*)&ble_svc_custom_uuid, // Указатель на UUID
           PRIMARY_SERVICE,
           5, // Максимальное количество атрибутов
           &custom_svc->svc_handle)) {
       FURI_LOG_E(TAG, "Failed to add custom service");
       free(custom_svc);
       return NULL;
   }
   FURI_LOG_I(TAG, "Custom service added successfully, handle: %d", custom_svc->svc_handle);
    // Параметры характеристики



    /*
typedef struct {
    const char* name;
    BleGattCharacteristicDescriptorParams* descriptor_params;
    union {
        struct {
            const uint8_t* ptr;
            uint16_t length;
        } fixed;
        struct {
            cbBleGattCharacteristicData fn;
            const void* context;
        } callback;
    } data;
    Char_UUID_t uuid;
    // Some packed bitfields to save space
    BleGattCharacteristicDataType data_prop_type : 2;
    uint8_t is_variable                          : 2;
    uint8_t uuid_type                            : 2;
    uint8_t char_properties;
    uint8_t security_permissions;
    uint8_t gatt_evt_mask;
} BleGattCharacteristicParams;*/
   
    BleGattCharacteristicParams char_params = {
        .name = "DataChar",
        .uuid_type = UUID_TYPE_16,
        .uuid = {.Char_UUID_16 = 0xABCD},
        .data_prop_type = FlipperGattCharacteristicDataCallback,
        .data = {
            .callback = {
                .fn = char_read_callback, // Исправленная функция
                
                .context = custom_svc
            },
            
        },
        
        .char_properties = CHAR_PROP_READ | CHAR_PROP_WRITE,
        .security_permissions = ATTR_PERMISSION_NONE,
        .gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE,
        .is_variable = CHAR_VALUE_LEN_CONSTANT,
        
        
        .descriptor_params = NULL
    
    
    };

    FURI_LOG_I(TAG, "Characteristic UUID: 0x%04X", char_params.uuid.Char_UUID_16);
    FURI_LOG_I(TAG, "Characteristic properties: 0x%02X", char_params.char_properties);
    
    // Добавление характеристики
    FURI_LOG_I(TAG, "Before ble_gatt_characteristic_init");
    BleGattCharacteristicInstance char_instance;
    
    ble_gatt_characteristic_init(custom_svc->svc_handle, &char_params, &char_instance);
    
    FURI_LOG_I(TAG, "After ble_gatt_characteristic_init");
    
    custom_svc->char_handle = char_instance.handle;
    //app->char_handle = char_instance.handle;
    //FURI_LOG_I(TAG, "!!!! Characteristic handle: %d", char_instance.handle);
    FURI_LOG_I(TAG, "!!!! Characteristic handle: %d", custom_svc->char_handle);
    
    // Регистрация обработчика событий
    custom_svc->event_handler =
        ble_event_dispatcher_register_svc_handler(ble_custom_service_event_handler, custom_svc);
    if (!custom_svc->event_handler) {
        FURI_LOG_E(TAG, "Failed to register event handler");
        return NULL;
    }
    FURI_LOG_I(TAG, "Event handler registered: %p", custom_svc->event_handler);
    
    FURI_LOG_I(TAG, "BLE custom service started successfully");
    return custom_svc;
}