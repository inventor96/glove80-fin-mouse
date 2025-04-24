[Source](https://www.studiopieters.nl/esp32-c3-pinout/)

# Analogue to Digital Converter (ADC)
The ESP32-C3 has 2 × 12-bit SAR ADC’s, up to 6 channels (while the ESP8266 only has 1x 10 bits ADC). These are the GPIO’s that can be used as ADC and respective channels.

- ADC1_CH0 (GPIO 0)
- ADC1_CH1 (GPIO 1)
- ADC1_CH2 (GPIO 2)
- ADC1_CH3 (GPIO 3)
- ADC1_CH4 (GPIO 4)
- ADC2_CH0 (GPIO 5)

ESP32-C3 integrates two 12-bit SAR ADCs.
- ADC1 supports measurements on 5 channels, and is factory-calibrated.
- ADC2 supports measurements on 1 channel, and is not factory-calibrated.

ESP32-C3 has 22 GPIO pins which can be assigned various functions by configuring corresponding registers. Besides digital signals, some GPIO’s can also be used for analogue functions, such as ADC.

# How to use the ESP32-C3 ADC pins
The ADC input channels have a 12 bit resolution. This means that you can get analogue readings ranging from 0 to 4095, in which 0 corresponds to 0V and 4095 to 3.3V. You also have the ability to set the resolution of your channels on the code, as well as the ADC range.

**Note:** The ESP32-C3 ADC pins don’t have a linear behaviour. You probably won’t be able to distinguish between 0 and 0.1V, or between 3.2 and 3.3V. You need to keep that in mind when using the ADC pins.