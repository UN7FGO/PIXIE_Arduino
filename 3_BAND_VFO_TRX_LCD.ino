// http://un7fgo.gengen.ru (C) 2020
// https://github.com/UN7FGO 
// 
// Basic, 3-band frequency synthesizer for PIXIE transceiver.
// The program uses the hardware feature of PIXIE,
// associated with the implementation of the frequency shift reception / transmission
//
// Базовый, 3-х диапазонный синтезатор частоты для трансивера PIXIE.
// В программе используется аппаратная особенность PIXIE, 
// связанная с реализацией сдвига частоты прием/передача

// Подключаем библиотеку для нашего LCD дистплея, подключенного по I2C протоколу
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RotaryEncoder.h>          // библиотека для энкодера


// Инициализируем дисплей 4х16, подключенный по I2C
LiquidCrystal_I2C lcd(0x27,24,4); 

// Описываем подключение модуля AD9850
#define W_CLK 8
#define FQ_UD 7
#define DATA  6 
#define RESET 5
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
 
// Определяем контакты, к которым у нас подключен энкодер
#define ENC_CLK_PIN 11
#define ENC_DT_PIN  10
#define ENC_SW_PIN  9

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
long int current_freq;
long int old_freq;
long int pressed;
int pinALast;  
int aVal;
int Band;
int nfreq;
int dfreq;
int Pos;
/* =================================================== */
void setup() {
  
  // все связанное с энкодером
  pinMode (ENC_CLK_PIN,INPUT);
  pinMode (ENC_DT_PIN,INPUT);
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

  lcd.begin();                     
  lcd.backlight();// Включаем подсветку дисплея
  lcd.clear();
  Refresh_LCD();

}

void loop() {
  // Если частота у нас изменилась, 
  // то обновляем ее значение на индикаторе и на синтезаторе
  if ( current_freq != old_freq ) {
    sendFrequency(current_freq);
    old_freq = current_freq;
    Refresh_LCD();
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
      Refresh_LCD();      
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
      Refresh_LCD();
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

// Передаем байт побитно, начиная с младшего бита, в AD9850, по последовательному интерфейсу 
// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

// расчитываем коэффициент деления для формирования AD9850 нужной нам частоты
// передаем нужные коэффициенты на AD9850
// frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
  for (int b=0; b<4; b++, freq>>=8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}


void Refresh_LCD()
{
  String S, Ss;
  S = "Band: " + name_band[Band] + "  ";
  lcd.setCursor(0, 0);
  lcd.print(S);
  
  lcd.setCursor(0, 1);
  S = "Freq: " + String(current_freq/1000000);
  Ss = String(current_freq/1000 - int(current_freq/1000000)*1000);
  while (Ss.length() < 3) {
    Ss = "0" + Ss;
  }
  S = S + "." + Ss;
  Ss = String(current_freq % 1000);
  while (Ss.length() < 3) {
    Ss = "0" + Ss;
  }
  S = S + "." + Ss + " ";
  lcd.print(S);
  
  lcd.setCursor(0, 2);
  lcd.print("Step: ");
  lcd.print(dfreq);
  lcd.print("    ");
  
  lcd.setCursor(0, 3);
  lcd.print("VCC ");
  lcd.print(float(int(analogRead(VOLT_PIN)*150/1023)/10) );
  lcd.print(" V  ");
}
