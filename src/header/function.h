void mainSetUp();          // Стартовые настройки
float getPressure();       // Получение давления в барах
float getFilterPressure(); // Проверка полученного давления
void closeValve();         // Функция ЗАКРЫТИЯ клапана
void openValve();          // Функция ОТКРЫТИЯ клапана
void mainLoop();           // Главный цикл опроса и управления клапаном