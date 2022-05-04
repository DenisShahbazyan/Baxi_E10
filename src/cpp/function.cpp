#include <Arduino.h>
#include "./header/data_sensors.h"

/*
 * Стартовые настройки проекта / датчиков.
 * Устанавливаем скорость серийного порта ардуино.
 * Заставляем работать пины как входы / выходы (INPUT / OUTPUT.
 * Устанавливаем стартовое положение пинов.
 */
void mainSetUp()
{
    Serial.begin(9600);

    pinMode(PIN_RELAY_00, OUTPUT);
    digitalWrite(PIN_RELAY_00, HIGH);

    pinMode(PIN_PRESSURE_SENSOR, INPUT);
}

/*
 * Функция опрашивает датчик давления, и отдает значение в Bar. По
 * умолчанию датчик отдает значение в условных единицах в диапозоне
 * ~100 ... ~900.
 * Соотношение условных единиц и даления в Bar можно найти в папке
 * проекта ./data/Pressure transducer logging.xlsx.
 */
float getPressure()
{
    // TODO: Протестировать показания датчика с насосом!!!
    int pressurePascal = analogRead(PIN_PRESSURE_SENSOR);
    //  float pressureBar = map(pressurePascal, 102, 920, 0, 12);
    float pressureBar = ((pressurePascal - 102) * 12 / 819); // Проверить формулу.
    return pressureBar;
}

/*
 * Функция опрашивает датчик давления 10 раз, и отдает среднее значение,
 * если ((Максимальное - Минимальное) < Погрешности), отдаем усредненное
 * значение, иначе отдаем 99,9 - считаем это значение за ошибку.
 */
float getFilterPressure()
{
    float ERR = 99.9;         // Ошибка
    float deviationBar = 1.0; // Макс. отклонение в давлении

    byte SIZE = 10;
    float value[SIZE];

    for (byte i = 0; i < SIZE; i++)
        value[i] = getPressure();

    float min_value = value[0];
    float max_value = value[0];
    float avg_value = 0;
    for (byte i = 0; i < SIZE; i++)
    {
        if (value[i] < min_value)
            min_value = value[i];
        if (value[i] > max_value)
            max_value = value[i];
        avg_value += value[i];
    }
    avg_value /= SIZE;

    if ((max_value - min_value) < deviationBar)
        return avg_value;
    return ERR;
}

/* Функция ЗАКРЫТИЯ клапана */
void closeValve()
{
    digitalWrite(PIN_RELAY_00, HIGH);
}

/* Функция ОТКРЫТИЯ клапана */
void openValve()
{
    digitalWrite(PIN_RELAY_00, LOW);
}

/*
 * Основной цикл програмы.
 * Опрашивает датчик давления с заданным интервалом, и проверяет границы
 * давления. Давление в системе должно быть от ~1.5 ... ~3.0 Bar.
 *
 * Если давление больше минимального, закрываем клапан (ничего не делаем).
 * Если давление меньше минимального - открываем клапан и в цикле ждем
 * 500 мс, после ожидания заного проверяем давление, если больше или равно
 * необходимо закрываем клапан, иначе держим клапан открытым и снова ждем
 * 500 мс.
 *
 * При подкачке давления проверяем давление, чтобы не скакнуть выше максимума.
 * Порог накачки давления должен быть ниже максимума.
 */
void mainLoop()
{
    float MIN_PRESSURE = 1.5;  // Минимальное давление в системе
    float NEED_PRESSURE = 2.0; // Необходимое давление в системе

    int DELAY = 500;            // Задержка для накачки системы в цикле
    int DELAY_LOOP = 1000 * 30; // Задержка главного цикла

    float pressure = getFilterPressure();
    if (pressure >= MIN_PRESSURE)
        closeValve();

    if (pressure < MIN_PRESSURE)
    {
        openValve();
        while (pressure <= NEED_PRESSURE)
        {
            delay(DELAY);
            pressure = getFilterPressure();
        }
        closeValve();
    }

    delay(DELAY_LOOP);
}