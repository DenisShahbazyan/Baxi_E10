#include <Arduino.h>
#include "./header/data_sensors.h"

/*
 * Объявление констант
 */
const int SLEEP_PRESSURE_SENSOR = 1000 * 1; // каждые 30 секунд отпрашивать датчик давления

/*
 * Объявление переменнных
 */
unsigned long lastTimePressure = 0; // millis для опроса датчика давления

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
    float ERR = 99.9;
    float deviationBar = 1.0;

    byte SIZE = 10;
    float value[SIZE];

    // Создаем массив с значениями.
    for (byte i = 0; i < SIZE; i++)
    {
        value[i] = getPressure();
    }

    // Находим Max, Min, Avg значения.
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

/*
 *
 */
void printToSerialPort()
{
    if (millis() - lastTimePressure > SLEEP_PRESSURE_SENSOR)
    {
        lastTimePressure = millis();

        Serial.println(getFilterPressure());
    }
}