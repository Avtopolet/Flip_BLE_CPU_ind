# Что это такое?
Это приложение для Flipper Zerо, которое показывает загрузку процессора компютера, когда вы сидите на работе за компом а Флиппердос лежит мирно рядом и демонстрирует то, как вы усердно работаете.

# Как скомпилировать?
Приложение реализует кастомный сервис с использованием функций, которые закрыты в публичном АПИ (может не случайно закрыты ?! :))<br />
Содержимое нужно поместить в ../application_user/ble_reader/.<br />
В файле манифста есть секция fap_private_libs. Там нужно указать пути к используемым библиотекам. Да, там абсолютные пути, наверное можно задать и относительные, проверьте это место,
иначе не соберется. Дальше выполняем **./fbt  launch APPSRC=applications_user/ble_reader**. Для логов можно запустить **./fbt cli** далее **log** (чтобы запустить, закройте приложение, запустите лог и потом опять запустите приложение)

# Как вот прям быстро проверить?
Берем телефон, запускаем Light Blue, подключаемся, находим сервис, находим характеристику (приложение должно быть запущено). Пишем в характеристику что то типа 0х240033 (24 - это наследие от другого проекта, второй и третий байты это число загрузки процессора).
На экранчике должно измениться число и нарисоваться столбик.

# Как слать данные с компьютера?
Установить недостающие либы для питона.
Дальше сложности, готовьте бубен.<br /> Я не разобрался как сделать так, чтобы нормально работало. При сопряжении Флиппер предоставляет код, который нужно ввести с компьютера.
На этом этапе возможно падение приложения, desctop app может не находить Флиппер или завершаться с ошибкой "Характеристика не найдена"
Какделал я. Разрывал соединения с Флиппера, удалял Флиппер с компа. Запускал приложение на Флиппере, потом запускал скрипт. Далее либо всё запускалось и я смахивал сообщение о коде на Флиппере (но и вводил код, так тоже работало) либо характеристика не находилась или приложение падало. Тут скорее всего будет несколько итераций. Скорее всего ошибки десктопного приложения связаны с тем, что комп не находит нужный сервис или сервис не создан к моменту соединения.<br /> После успешного соединения всё работает стабильно. Может, с помошью сообщества получится докрутить этот момент.

> [!CAUTION]
> Ребят, на свой страх и риск :)

