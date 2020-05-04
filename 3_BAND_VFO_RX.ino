// http://un7fgo.gengen.ru (C) 2019
// https://github.com/UN7FGO 
// 

// Подключаем библиотеку для нашего 7-сегментного дисплея на 4 цифры
#include <TM1637Display.h>
// Контакты для подключения нашего 7-сегментного дисплея
#define D_CLK 8
#define D_DIO 7
// Создаем объект связанный с дисплеем
TM1637Display display(D_CLK, D_DIO);

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

// массив "текущих частот" 
long int cur_freq[3]  = {1900000, 3600000, 7100000};
// массив "максимальных частот" 
long int max_freq[3]  = {2000000, 3800000, 7300000};
// массив "минимальных частот" 
long int min_freq[3]  = {1800000, 3500000, 7000000};

// переменные для работы
long int current_freq;
long int old_freq;
int pinALast;  
int aVal;
int Band;

/* =================================================== */
void setup() {
  // устанавливаем яркость дисплея
  display.setBrightness(1);
  // все связанное с энкодером
  pinMode (ENC_CLK_PIN,INPUT);
  pinMode (ENC_DT_PIN,INPUT);
  pinMode (ENC_SW_PIN,INPUT);
  pinALast = digitalRead(ENC_CLK_PIN);   

  // частота "по умолчанию"
  old_freq = 0;
  Band = 2;
  current_freq = cur_freq[Band];
}

void loop() {
  if ( current_freq != old_freq ) {
    display.clear();
    display.showNumberDec(int(current_freq / 1000));
    sendFrequency(current_freq);
    old_freq = current_freq;
  }

  if (digitalRead(ENC_SW_PIN) == 0) {
    cur_freq[Band] = current_freq;
    Band +=1;
    if ( Band > 2 ) {
      Band = 0;
    }
    current_freq = cur_freq[Band];
    delay (500);
  }

  aVal = digitalRead(ENC_CLK_PIN);
  if (aVal != pinALast){ 
    if (digitalRead(ENC_DT_PIN) == aVal) {
       current_freq += 100;
       if ( current_freq > max_freq[Band] ) {
         current_freq = max_freq[Band];
       }
     } else {
       current_freq -= 100;
       if ( current_freq < min_freq[Band] ) {
         current_freq = min_freq[Band];
       }
     }
  }
  pinALast = aVal;
}

// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    digitalWrite(DATA, data & 0x01);
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

 // frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
  for (int b=0; b<4; b++, freq>>=8) {
    tfr_byte(freq & 0xFF);
  }
  tfr_byte(0x000);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}
