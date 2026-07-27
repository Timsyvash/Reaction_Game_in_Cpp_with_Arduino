#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

const short matrix_pin = 10;
const short max_count_leds = 64;
const short rows = 8;
const short cols = 8;
const short button_pin = 6;

unsigned long last_time = 0;
unsigned long speedDelay = 200;

// Змінні для керування ефектами анімації
unsigned long effect_time = 0;
short effect_state = 0; // 0 - гра, 1 - перемога, 2 - програш
short flash_count = 0;

// Межі руху (наприклад, перший рядок матриці від 0 до 7)
short pos = 0;
short dir = 1;
const short min_pos = 0;
const short max_pos = 7;

// Центральні пікселі для перевірки (індекси 3 та 4 для перших 8 діодів)
const short target_1 = 3;
const short target_2 = 4;

Adafruit_NeoPixel matrix(max_count_leds, matrix_pin, NEO_GRB + NEO_KHZ800);

void setup()
{
  matrix.begin();
  matrix.clear();
  matrix.show();
  pinMode(button_pin, INPUT_PULLUP);
}

void loop()
{
  int buttonState = digitalRead(button_pin);

  // --- РЕЖИМ ГРИ ---
  if (effect_state == 0)
  {
    // Рух точки за таймером
    if (millis() - last_time >= speedDelay)
    {
      last_time = millis();

      matrix.clear();
      matrix.setPixelColor(pos, matrix.Color(0, 0, 255)); // Синя точка
      matrix.show();

      // Зміна напрямку руху на краях лінії
      if (pos == min_pos && dir == -1)
        dir = 1;
      else if (pos == max_pos && dir == 1)
        dir = -1;

      // Спочатку перевіряємо кнопку (поки pos не змінився),
      // а рух робимо в самому кінці такту
      pos += dir;
    }

    // Перевірка натискання кнопки
    if (buttonState == LOW)
    {
      // Повертаємо pos назад на 1 крок для перевірки,
      // бо в кінці минулого такту ми його вже зсунули наперед
      short current_pos = pos - dir;

      if (current_pos == target_1 || current_pos == target_2)
      {
        effect_state = 1; // Запуск ефекту перемоги
      }
      else
      {
        effect_state = 2; // Запуск ефекту програшу
      }
      flash_count = 0;
      effect_time = millis();
    }
  }
  // --- РЕЖИМ ЕФЕКТУ ПЕРЕМОГИ (Зелений) ---
  else if (effect_state == 1)
  {
    if (millis() - effect_time >= 300)
    {
      effect_time = millis();
      flash_count++;

      if (flash_count % 2 != 0)
      {
        matrix.fill(matrix.Color(0, 255, 0)); // Увімкнути зелений
      }
      else
      {
        matrix.clear(); // Вимкнути
      }
      matrix.show();

      if (flash_count >= 6)
      {                   // 3 спалахи (увімкнено/вимкнено * 3)
        effect_state = 0; // Повернення до гри
        pos = min_pos;    // Скидання позиції точки
        dir = 1;
      }
    }
  }
  // --- РЕЖИМ ЕФЕКТУ ПРОГРАШУ (Червоний) ---
  else if (effect_state == 2)
  {
    if (millis() - effect_time >= 300)
    {
      effect_time = millis();
      flash_count++;

      if (flash_count % 2 != 0)
      {
        matrix.fill(matrix.Color(255, 0, 0)); // Увімкнути червоний
      }
      else
      {
        matrix.clear(); // Вимкнути
      }
      matrix.show();

      if (flash_count >= 6)
      {
        effect_state = 0; // Повернення до гри
        pos = min_pos;
        dir = 1;
      }
    }
  }
}
