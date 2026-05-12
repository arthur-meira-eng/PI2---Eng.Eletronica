#ifndef TOF_VL53L0X_H
#define TOF_VL53L0X_H

#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

void initToF();
void lerSensoresToF(uint16_t &dist1, uint16_t &dist2);

#endif