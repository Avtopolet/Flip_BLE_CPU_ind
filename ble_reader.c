/*********************************************
 * Flip.x0 Tutorial
 *                                by M1ch3al
 * 0x01 - my first app
 ********************************************
 */

 #include <stdio.h>
 #include <furi.h>
 #include <gui/gui.h>
 #include "ble_custom_service.h"
#include "ble_reader.h"

#include <furi_ble/event_dispatcher.h> // Для обработки событий BLE
 
 // This function is for drawing the screen GUI, everytime
 // the Flip.x0 refresh the display

 typedef struct {
    uint8_t char_data[20]; // Буфер для данных характеристики
    uint16_t char_data_len; // Длина данных
} BleReaderContext;

/*
static BleEventAckStatus ble_event_handler(void* event, void* context) {
    BleReaderApp* app = context;
    aci_gatt_attribute_modified_event_rp0* evt = event;

    if (evt->Attr_Handle == app->char_handle) {
        memcpy(app->char_data, evt->Attr_Data, evt->Attr_Data_Length);
        app->char_data_len = evt->Attr_Data_Length;
        view_port_update(app->view_port);
    }
    return BleEventNotAck;
}
*/

static void draw_callback(Canvas* canvas, void* ctx) {
    BleReaderApp* app = (BleReaderApp*)ctx;
    if (!app) return;

    // Очищаем экран
    canvas_clear(canvas);

    // Заголовок
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 10, "CPU Load");

    // Отображаем текущий процент
    char percent_str[10];
    snprintf(percent_str, sizeof(percent_str), "%d%%", app->cpu_percent);
    canvas_draw_str(canvas, 80, 10, percent_str);

    // Рисуем гистограмму ----------------------------------------------------
    const uint8_t graph_x = 5;       // Начало по X
    const uint8_t graph_y = 60;      // Начало по Y
    const uint8_t graph_height = 48; // Максимальная высота
    const uint8_t bar_width = 3;     // Ширина столбика
    const uint8_t bar_spacing = 1;   // Расстояние между столбиками

    // Рисуем каждый столбик
    for (uint8_t i = 0; i < HISTORY_SIZE; i++) {
        // Вычисляем индекс для истории (движение справа налево)
        uint8_t index = (app->history_index + i) % HISTORY_SIZE;

        // Высота столбика (0-100% -> 0-30px)
        uint8_t bar_height = (app->cpu_history[index] * graph_height) / 100;

        // Позиция столбика
        uint8_t x = graph_x + i * (bar_width + bar_spacing);
        uint8_t y = graph_y - bar_height;

        // Рисуем столбик
        canvas_draw_box(canvas, x, y, bar_width, bar_height);
    }
    
    /*
    canvas_clear(canvas);
     // Простая отрисовка для теста
    // canvas_draw_str(canvas, 10, 10, "Hello, Flipper!");
    
    // Выводим полученные данные
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "BLE Data:");
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Len: %d", app->char_data_len);
    canvas_draw_str(canvas, 2, 25, buffer);
    
    if(app->char_data_len > 0) {
        snprintf(buffer, sizeof(buffer), "Data: %02X %02X %02X", 
                app->char_data[0], app->char_data[1], app->char_data[2]);
        canvas_draw_str(canvas, 2, 40, buffer);
    }
        */
}
 
 // This function is an handler for the user input (the buttons on the right
 // of the Flip.x0 used for navigate, confirm and back)
 /*
 static void input_callback(InputEvent* input_event, void* ctx) {
     furi_assert(ctx);
     FuriMessageQueue* event_queue = ctx;
     furi_message_queue_put(event_queue, input_event, FuriWaitForever);
 }
 */
 // Main entry of the application as defined inside the application.fam
 static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}
 
 int32_t my_first_app_main(void* p) {
     UNUSED(p);
     FURI_LOG_I("BleReader", "Begin");
     InputEvent event;
    // Создаем контекст
           // Выделение памяти для приложения
        BleReaderApp* app = malloc(sizeof(BleReaderApp));
        if (!app) {
            FURI_LOG_E("BleReader", "Failed to allocate memory for app");
        
        } else {FURI_LOG_I("BleReader", "Sucessful allocate memory for app");}
    
        
        // Инициализация полей структуры
        
        app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
        if (!app->event_queue) {
            FURI_LOG_E("BleReader", "Failed to allocate event queue");
            free(app);   
        }else {FURI_LOG_I("BleReader", "Sucessful allocate memory for event queue");}

        
        
        app->view_port = view_port_alloc();
          
    if (!app->view_port) {
        FURI_LOG_E("BleReader", "Failed to allocate view port");
        furi_message_queue_free(app->event_queue);
        free(app);} else {FURI_LOG_I("BleReader", "Sucessful view_port_alloc");}

        app->char_data_len = 0;
        app->char_handle = 0;
        FURI_LOG_I("BleReader", "Fields inited");
    
    
    
    // Инициализируем GUI
    
    view_port_draw_callback_set(app->view_port, draw_callback, app);
    FURI_LOG_I("BleReader", "view_port_draw_callback_set complite");
    
    Gui* gui = furi_record_open(RECORD_GUI);

    if (!gui) {
                FURI_LOG_E("BleReader", "Failed to open GUI record");
                furi_message_queue_free(app->event_queue);
                view_port_free(app->view_port);
                free(app);} else {FURI_LOG_I("BleReader", "Sucessful furi_record_open");}

    // Добавление view port в GUI
    FURI_LOG_I("BleReader", "Adding view port to GUI. GUI: %p, View port: %p", gui, app->view_port);

        
    
    
    
    gui_add_view_port(gui, app->view_port, GuiLayerFullscreen);
    FURI_LOG_I("BleReader", "GUI inited");    
    
   // FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    view_port_input_callback_set(app->view_port, input_callback, app->event_queue);


    /*
     // Current event of type InputEvent
     InputEvent event;
 
     // Event queue for 8 elements of size InputEvent
     FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
 
     // ViewPort is need to draw the GUI
     ViewPort* view_port = view_port_alloc();
 
     // we give to this [view_port_draw_callback_set] the function defined
     // before (draw_callback) for drawing the GUI on the Flip.x0 display
     view_port_draw_callback_set(view_port, draw_callback, NULL);
 
     // The same concept is with [view_port_input_callback_set] associated to the (input_callback)
     // function defined before.
     view_port_input_callback_set(view_port, input_callback, event_queue);
 
     // You need to create a GUI structure and associate it to the viewport previously defined
     Gui* gui = furi_record_open(RECORD_GUI);
     gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    */
    /*
     BleCustomService* custom_svc = ble_custom_service_start();
     if(!custom_svc) {
         FURI_LOG_E("BleReader", "Failed to start custom service");
         return -1;
     }
     */

     // Запускаем BLE-сервис и передаем контекст
   // Инициализация BLE
   BleCustomService* svc = ble_custom_service_start(app);
   if (!svc) {
    FURI_LOG_E("BleReader", "Failed to start custom BLE service");
    
}
   
   //ble_event_dispatcher_register_svc_handler(ble_event_handler, app);


   
     // Infinite loop...(like arduino or similar)
     while(true) {
         
        
        /*
        // We continue (indefinitely) to get out of the queue all the events stacked inside
         furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);
 
         // If the event from the queue is the press of the back button, we exit from the loop
         if(event.key == InputKeyBack) {
             break;

        */

        if(furi_message_queue_get(app->event_queue, &event, FuriWaitForever) == FuriStatusOk) {
            if(event.key == InputKeyBack){ break;}
            
            
        }
        // При получении данных через BLE обновляем экран
        view_port_update(app->view_port);



         
     }
 /*
     // once exit from the loop, we need to free resources:
     // clear all the element inside the queue
     furi_message_queue_free(event_queue);
 
     // We remove the gui from the associated view port
     gui_remove_view_port(gui, view_port);
 
     // Freeing up memory removing the view_port and close
     // the GUI record
     view_port_free(view_port);
     furi_record_close(RECORD_GUI);
     */
    // Очистка
    gui_remove_view_port(gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    free(app);
     return 0;
 }