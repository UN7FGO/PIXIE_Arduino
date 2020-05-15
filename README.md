# PIXIE_Arduino
Various VFO in Arduino for PIXIE transceiver

Here I post my attempts to make an additional VFO for the pixie transceiver.

Projects implemented on Arduino and AD9850 synthesizer.

The latest version with support for the LCD screen and the control of PDF on three bands.

This project is educational in nature and does not claim to be ideal in terms of electronics.

Versions of program:

1. 3_BAND_VFO_RX.ino - for recevier - PIXIE + Arduino Nano + AD9850 + Rotate encoder + display 7 segment 4 digit on TM1637
	- 3 band - 160-80-40 meter (switched by pressing the encoder button);
	- fixed frequency tuning step 100 Hz;
	- the indicator displays only the frequency in kHz
	https://github.com/UN7FGO/PIXIE_Arduino/blob/master/PIXIE_3_BAND_RX_Photo_2.jpg

2. 3_BAND_VFO_TRX.ino - for transceiver - PIXIE + Arduino Nano + AD9850 + Rotate encoder + display 7 segment 8 digit on MAX7219
	- 3 band - 160-80-40 meter (switches by long pressing of the encoder button);
	- variable frequency tuning step 10-100-1000-10000 Hz (switched by a short press of the encoder button);
	- The indicator shows the frequency in Hz;
	- With a short press of the encoder button, a new frequency step in Hz is displayed.
	https://github.com/UN7FGO/PIXIE_Arduino/blob/master/PIXIE_3_BAND_TRX_Photo_1.jpg

3. 3_BAND_VFO_TRX_LCD.ino - for transceiver - PIXIE + Arduino UNO + AD9850 +  Rotate encoder + I2C LCD display 4 row 20 column
	- 3 band - 160-80-40 meter (switches by long pressing of the encoder button);
	- variable frequency tuning step 1-10-100-1000-10000 Hz (switched by a short press of the encoder button);
	- The screen displays - band name, frequency, frequency tuning step, transceiver supply voltage;
	- External libraries are used to process the encoder and synthesizer AD9850.


Suggestions and criticism are welcome.
