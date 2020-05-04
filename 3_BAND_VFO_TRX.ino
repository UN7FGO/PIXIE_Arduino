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

// Подключаем библиотеку для нашего 7-сегментного индикатора на 8 цифр
#include "LedControl.h"
// Контакты для подключения нашего 7-сегментного индикатора
#define D_DIN 7
#define D_CS  8 
#define D_CLK 9
// Создаем объект связанный с дисплеем
LedControl lc=LedControl(D_DIN,D_CS,D_CLK,1);

// Описываем подключение модуля AD9850
#define W_CLK 5
#define FQ_UD 4
#define DATA  3 
#define RESET 2
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }
 
// Определяем контакты, к которым у нас подключен энкодер
#define ENC_CLK_PIN 12
#define ENC_DT_PIN  11
#define ENC_SW_PIN  10

// Количество диапазонов и массивы с их параметрами 
#define MAXBAND 3
// массив "текущих частот" 
long int cur_freq[3]  = {1900000, 3600000, 7100000};
// массив "максимальных частот" 
long int max_freq[3]  = {2000000, 3800000, 7300000};
// массив "минимальных частот" 
long int min_freq[3]  = {1800000, 3500000, 7000000};

// максимальное количество шагов перестройки частоты
#define MAXFREQ 4
// массив "шагов перестройки" в Герцах
long int d_freq[4]  = {10000, 1000, 100, 10};

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
/* =================================================== */
void setup() {
  // переводим индикатор в активный
  lc.shutdown(0,false);
  // устанавливаем яркость дисплея
  lc.setIntensity(0,8);
  // очищаем индикатор
  lc.clearDisplay(0);

  // все связанное с энкодером
  pinMode (ENC_CLK_PIN,INPUT);
  pinMode (ENC_DT_PIN,INPUT);
  pinMode (ENC_SW_PIN,INPUT);
  pinALast = digitalRead(ENC_CLK_PIN);   

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
}

void loop() {
  // Если частота у нас изменилась, 
  // то обновляем ее значение на индикаторе и на синтезаторе
  if ( current_freq != old_freq ) {
    lc_freq(current_freq,0);
    sendFrequency(current_freq);
    old_freq = current_freq;
  }

  // если наш трансивер перешел в режим передачи
  if (analogRead( TX_PIN ) == 0 ) {
    sendFrequency( (current_freq + TXSHIFT) );
    lc.clearDisplay(0);
    lc.setChar(0,6,'-',false);
    lc.setChar(0,5,'5',false);
    lc.setChar(0,4,'E',false);
    lc.setChar(0,3,'H',false);
    lc.setChar(0,2,'d',false);
    lc.setChar(0,1,'-',false);
    // ждем пока трансивер не перейдет в режим приема
    while (analogRead( TX_PIN ) == 0 ) {
      // 
    }
    // восстанавливаем на синтезаторе текущую частоту
    sendFrequency(current_freq);
    // обновляем информацию о частоте на индикаторе
    lc_freq(current_freq,0);
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
      delay (200);
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
      lc_freq(dfreq,2);
      lc.setChar(0,0,'H',false);
      // ждем 0.8 секунды
      delay (800);
      // выводим на индикатор текущую частоту
      lc_freq(current_freq,0);
    }
  }

  // обрабатываем энкодер
  aVal = digitalRead(ENC_CLK_PIN);
  // проверяем, был ли произведен поворот ручки энкодера
  if (aVal != pinALast){ 
    // определяем направление вращения энкодера
    if (digitalRead(ENC_DT_PIN) == aVal) {
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
  }
  pinALast = aVal;
  
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

// процедура вывода на 8-разрядный индикатор нужной цифры, со сдвигом от правого края
// не значащие нули не показываются
void lc_freq(long int fr, byte nn)
{
  byte dg;
  // очищаем экран
  lc.clearDisplay(0);
  // в цикле перебираем цифры числа, начиная с единиц
  for (int i = 0; i < (8-nn); i++) {
     // если цифры "закончились", то ничего не выводим
     if (fr > 0) {
       dg = fr % 10;
       lc.setDigit(0,i+nn,dg,false); 
       fr = fr / 10;
     }
  }
}
