# PIXIE_Arduino
Various VFO in Arduino for PIXIE transceiver

Here I post my attempts to make an additional VFO for the pixie transceiver.

Projects implemented on Arduino and AD9850 synthesizer.

The latest version with support for the LCD screen and the control of PDF on three bands.

This project is educational in nature and does not claim to be ideal in terms of electronics.

Versions of program:

1. 3_BAND_VFO_RX.ino - **for recevier** - PIXIE + Arduino Nano + AD9850 + Rotary encoder + display 7 seg. 4 dig. on TM1637
	- 3 band - 160-80-40 meter (switched by pressing the encoder button);
	- fixed frequency tuning step 100 Hz;
	- the indicator displays only the frequency in kHz
	https://github.com/UN7FGO/PIXIE_Arduino/blob/master/PIXIE_3_BAND_RX_Photo_2.jpg

2. 3_BAND_VFO_TRX.ino - **for transceiver** - PIXIE + Arduino Nano + AD9850 + Rotary encoder + display 7 seg. 8 dig. on MAX7219
	- 3 band - 160-80-40 meter (switches by long pressing of the encoder button);
	- variable frequency tuning step 10-100-1000-10000 Hz (switched by a short press of the encoder button);
	- The indicator shows the frequency in Hz;
	- With a short press of the encoder button, a new frequency step in Hz is displayed.
	https://github.com/UN7FGO/PIXIE_Arduino/blob/master/PIXIE_3_BAND_TRX_Photo_1.jpg

3. 3_BAND_VFO_TRX_LCD.ino - **for transceiver** - PIXIE + Arduino UNO + AD9850 + Rotary encoder + I2C LCD display 4 row 20 column
	- 3 band - 160-80-40 meter (switches by long pressing of the encoder button);
	- variable frequency tuning step 1-10-100-1000-10000 Hz (switched by a short press of the encoder button);
	- The screen displays - band name, frequency, frequency tuning step, transceiver supply voltage;
	- External libraries are used to process the encoder and synthesizer AD9850.


Suggestions and criticism are welcome.


**============================== R U S S I A N ==============================**


Вариант простого в реализации ГПД для трансивера PIXIE.

Тут будут размещаться мои варианты попыток реализовать ГПД для этого трансивера.
Какие то прототипы опробованы в "железе", какие то являются только "теоретическими" построениями.

ГПД построены на базе Arduino и синтезатора AD9850.
Однако вы можете использовать любой другой синтезатор, изменив только одну функцию установки частоты (в одном месте).

Последняя версия позволяет использовать жидкокристаллический дисплей и имеет выходы для переключения трех диапазонных ДПФ.

Проект реализован в образовательных целях и может содержать неточности в аппартной реализации самого трансивера или подключения к нему.

Версии программ, выложенные в этом репозитарии:

1. 3_BAND_VFO_RX.ino - **для приемника** 

***Используемые модули:***
	- трансивер PIXIE;
	- Arduino Nano;
	- синтезатор AD9850;
	- Энкодер;
	- 7-сегментный дисплей на 4 цифры, собранный на микросхеме TM1637.

***Возможности:***
	- 3 диапазона - 160-80-40 метров (переключаются нажатием на кнопку энкодера);
	- фиксированный шаг перестройки в 100 Гц (можно исправить в программе);
	- на индикаторе отображается частота в кГц
	https://github.com/UN7FGO/PIXIE_Arduino/blob/master/PIXIE_3_BAND_RX_Photo_2.jpg


2. 3_BAND_VFO_TRX.ino - **для трансивера**

***Используемые модули:***
- трансивер PIXIE;
- Arduino Nano; 
- синтезатор AD9850;
- Энкодер;
- 7-сегментный дисплей на 8 цифр, собранный на микросхеме MAX7219.

***Возможности:***/n
- 3 диапазона - 160-80-40 метров (переключаются долгим нажатием на кнопку энкодера);
- изменяемый шаг перстройки частоты 10-100-1000-10000 Гц (переключаются коротким нажатием на кнопку энкодера);
- при коротком нажатии кнопки энкодера, на индикаторе отображается выбранный шаг перестройки в Гц.
- на индикаторе отображается частота в Гц
https://github.com/UN7FGO/PIXIE_Arduino/blob/master/PIXIE_3_BAND_TRX_Photo_1.jpg


3. 3_BAND_VFO_TRX_LCD.ino - **для трансивера**

***Используемые модули:***
- трансивер PIXIE 
- Arduino Nano 
- синтезатор AD9850 
- Энкодер
- LCD дисплей 4 строки по 20 символов, подключенный по интерфейсу I2C
	
***Возможности:***
- 3 диапазона - 160-80-40 метров (переключаются долгим нажатием на кнопку энкодера);
- изменяемый шаг перстройки частоты 10-100-1000-10000 Гц (переключаются коротким нажатием на кнопку энкодера);
- На дисплее отображается: наименование диапазона, текущая частота, текущий шаг перестройки, напряжение питания трансивера;
- использованы внешние библиотеки для обработки энкодера и синтезатора AD9850.

ГПД может быть переделан под любые другие диапазоны, все параметры доступны в исходном тексте программы.

Критика и предложения приветствуются.
