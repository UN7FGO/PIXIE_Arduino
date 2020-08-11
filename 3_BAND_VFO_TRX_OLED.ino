// http://un7fgo.gengen.ru (C) 2019
// https://github.com/UN7FGO 
// 
// Basic, 3-band frequency synthesizer for PIXIE transceiver.
// The program uses the hardware feature of PIXIE,
// associated with the implementation of the frequency shift reception / transmission
//
// Базовый, 3-х диапазонный синтезатор частоты для трансивера PIXIE.
// В программе используется аппаратная особенность PIXIE, 
// связанная с реализацией сдвига частоты прием/передача

// Подключаем библиотеку для нашего OLED дистплея, подключенного по I2C протоколу
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RotaryEncoder.h>          // библиотека для энкодера
#include <AD9850.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Описываем подключение модуля AD9850
#define W_CLK_PIN 5
#define FQ_UD_PIN 4
#define DATA_PIN  3
#define RESET_PIN 2
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
int phase = 0;
 
// Определяем контакты, к которым у нас подключен энкодер
#define ENC_CLK_PIN 9
#define ENC_DT_PIN  7
#define ENC_SW_PIN  8

RotaryEncoder encoder(ENC_DT_PIN, ENC_CLK_PIN);   

// Определяем контакт, к которому подключен делитель входного напряжения 1:3
#define VOLT_PIN A1

// Количество диапазонов и массивы с их параметрами 
#define MAXBAND 3
// массив "текущих частот" 
long int cur_freq[MAXBAND]  = {1900000, 3600000, 7100000};
// массив "максимальных частот" 
long int max_freq[MAXBAND]  = {2000000, 3800000, 7300000};
// массив "минимальных частот" 
long int min_freq[MAXBAND]  = {1800000, 3500000, 7000000};
// массив "названий диапазонов" 
String name_band[MAXBAND]  = {"160 meter", "80 meter","40 meter"};
// массив "выводов для перключения ДПФ приемника" 
long int lbp_pin[MAXBAND]  = {2, 3, 4};

// максимальное количество шагов перестройки частоты
#define MAXFREQ 5
// массив "шагов перестройки" в Герцах
long int d_freq[MAXFREQ]  = {10000, 1000, 100, 10, 1};

// Контакт сигнализирующий о переходе трансивера на передачу
// В режиме приема на нем имеется какое-то напряжение, 
// При переходе на передачу, контакт замыкается на землю
#define TX_PIN A1
// Сдвиг частоты в Герцах, при переходе на передачу (может быть и отрицательным)
#define TXSHIFT 800

// переменные для работы
unsigned long int k, fr, current_freq;
unsigned long int old_freq;
unsigned long int pressed;
int pinALast;  
int aVal;
int Band;
int nfreq;
int dfreq;
int Pos;
/* =================================================== */
void setup() {

  Serial.begin(9600);
  // все связанное с энкодером
  pinMode (ENC_CLK_PIN,INPUT_PULLUP);
  pinMode (ENC_DT_PIN,INPUT_PULLUP);
  pinMode (ENC_SW_PIN,INPUT_PULLUP);

  encoder.setPosition(0);

  // "старая" частота "по умолчанию"
  old_freq = 0;
  // Текужий диапазон
  Band = 2;
  // Текущая частота
  current_freq = cur_freq[Band];
  // Текущий номер шага изменения частоты
  nfreq = 2;
  // Текущий шаг изменения частоты
  dfreq = d_freq[nfreq];

  DDS.begin(W_CLK_PIN, FQ_UD_PIN, DATA_PIN, RESET_PIN);
  DDS.calibrate(125000000);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
  }
  Refresh_Display();
}

void loop() {
  
  // Если частота у нас изменилась, 
  // то обновляем ее значение на индикаторе и на синтезаторе
  if ( current_freq != old_freq ) {
    sendFrequency(current_freq);
    old_freq = current_freq;
    Refresh_Display();
  }

  // если наш трансивер перешел в режим передачи
  if (TXSHIFT != 0) {
    if (analogRead( TX_PIN ) == 0 ) {
      sendFrequency( (current_freq + TXSHIFT) );
      // ждем пока трансивер не перейдет в режим приема
      while (analogRead( TX_PIN ) == 0 ) {
        // 
      }
      // восстанавливаем на синтезаторе текущую частоту
      sendFrequency(current_freq);
    }
  }

  // обрабатываем кнопку энкодера
  if (digitalRead(ENC_SW_PIN) == 0) {
    // запомнаем время нажатия кнопки
    pressed = millis();
    // ждем, пока кнопку отпустят
    while (digitalRead(ENC_SW_PIN) == 0) {
    }
    // считаем время, сколько была нажата кнопка
    pressed = millis() - pressed;
    // если время нажатия больше 1 секунды, то переключаем диапазон
    if ( pressed > 1000 ) {
      // запоминаем текущую частоту на текущем диапазоне
      cur_freq[Band] = current_freq;
      // увеличиваем номер диапазона
      Band +=1;
      // если номер больше максимального, возвращаемся в начало
      if ( Band == MAXBAND ) {
        Band = 0;
      }
      // считываем текущую частоту выбранного диапазона
      current_freq = cur_freq[Band];
      // Переключаем входной ДПФ приемника
      for (int i=0; i<MAXBAND; i++){
       digitalWrite(lbp_pin[i], LOW);
      } 
      digitalWrite(lbp_pin[Band], HIGH);
      Refresh_Display();      
    } else {
      // если кнопка былв нажаты менее 1 секунды, меняем шаг перестройки
      // переходим на следующий шаг
      nfreq += 1;
      // если шаг больше возможного, переходим к первому значению
      if ( nfreq == MAXFREQ ) {
        nfreq = 0;
      }
      // запоминаем выбранный шаг перестройки
      dfreq = d_freq[nfreq];
      // выводим на индикатор информацию о выбранном шаге перестройки
      Refresh_Display();
    }
  }

  // обрабатываем энкодер
  encoder.tick();
  Pos = encoder.getPosition();
  // проверяем, был ли произведен поворот ручки энкодера
  if (Pos != 0){ 
    // определяем направление вращения энкодера
    if (Pos < 0) {
       // повернули энкодер "по часовой стрелке" (CW)
       current_freq += dfreq;
       // не даем частоте уйти за верхний предел диапазона
       if ( current_freq > max_freq[Band] ) {
         current_freq = max_freq[Band];
       }
     } else {
       // повернули энкодер "против часовой стрелки" (CCW)
       current_freq -= dfreq;
       // не даем частоте уйти за нижний предел диапазона
       if ( current_freq < min_freq[Band] ) {
         current_freq = min_freq[Band];
       }
     }
     encoder.setPosition(0);
  }
}

void sendFrequency(double frequency) {
  DDS.setfreq(frequency, 0);
}

long int intpow(int p) {
  long int k = 1;
  for (int j=1; j<p; j++) {
    k = k * 10;
  }
  return k;
}
void Refresh_Display()
{
  int ost,yy;
  String S, Ss;
  display.clearDisplay();
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setTextSize(2);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);    
  S = name_band[Band];
  display.print(S);
  
  display.setCursor(0, 16);    
    Ss = ""; 
    fr = current_freq;
    for (int i=8; i>0; i--) {
      k = intpow(i);
      ost = fr / k;
      Ss += ost;
      fr = fr % k; 
      if (i == 7 || i == 4) {
        Ss += ".";    
      }
    }
    for (int i=0; i<=9; i++) {
      display.print(Ss[i]);
    }
  
  display.setCursor(0, 32);    
  display.print(F("St: "));
  display.print(dfreq);
  
  display.setCursor(0, 48);
  display.print(F("VCC "));
  display.print(float(int(analogRead(VOLT_PIN)*150/1023)/10) );
  display.print(F("v"));
  display.display();
  
}
