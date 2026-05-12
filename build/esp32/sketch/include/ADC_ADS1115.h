#line 1 "/home/yaba/Sandbox/PI2---Eng.Eletronica/PI2/include/ADC_ADS1115.h"
#ifndef ADC_ADS1115_H
#define ADC_ADS1115_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>

void initADC();
float lerTensaoBateria(int bateria); 
float lerCorrenteMotor(int motor);

#endif