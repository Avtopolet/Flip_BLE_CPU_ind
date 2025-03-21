/*********************************************
 * BlE indicator for Flipper by Avtopolet
 *                               
 * 
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


// Коллбек отрисовки
static void draw_callback(Canvas* canvas, void* ctx) {
    BleReaderApp* app = (BleReaderApp*)ctx; // Долго разбирался с тем, что такое контекст.
											// Для себя понял, что это некая настраиваемая структура,
										    // через которую можно передавать что угодно из любого места
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
    

 // Обработка кнопочек, это для кнопки "Назад, для выхода из приложения"
 static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}


// Точка входа, тут много всякой отладки, оставлю вам, может пригодится 
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




     // Запускаем BLE-сервис и передаем контекст
   
   BleCustomService* svc = ble_custom_service_start(app);
   if (!svc) {
    FURI_LOG_E("BleReader", "Failed to start custom BLE service");
    
}
   
   //ble_event_dispatcher_register_svc_handler(ble_event_handler, app);


   
     // Основной цикл
     while(true) {
         
        

        if(furi_message_queue_get(app->event_queue, &event, FuriWaitForever) == FuriStatusOk) {
            if(event.key == InputKeyBack){ break;}
            
            
        }
        // При получении данных через BLE обновляем экран
        view_port_update(app->view_port);



         
     }
 
    // Очистка
    gui_remove_view_port(gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    free(app);
     return 0;
 }