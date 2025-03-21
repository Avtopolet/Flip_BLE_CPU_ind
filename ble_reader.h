#ifndef BLE_READER_H
#define BLE_READER_H

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#define HISTORY_SIZE 32 // Количество сохраняемых значений
// Контекст приложения
typedef struct {
    ViewPort* view_port;       // Указатель на ViewPort для обновления экрана
    FuriMessageQueue* event_queue; // Очередь событий
    
    uint8_t char_data[32];     // Буфер для данных характеристики
    uint16_t char_data_len;    // Длина полученных данных
    uint16_t char_handle; // Добавьте это поле
    uint8_t cpu_percent; // Процент загрузки (0-100)
    uint8_t cpu_history[HISTORY_SIZE]; // История значений
    uint8_t history_index; // Текущий индекс в истории
} BleReaderApp;

#endif