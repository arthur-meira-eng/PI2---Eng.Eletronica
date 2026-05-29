#line 1 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/include/IMU_MPU6050.h"
#ifndef IMU_MPU6050_H
#define IMU_MPU6050_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

void initIMU();
void lerAceleracao(float &x, float &y, float &z);
void lerIMU(float &accX, float &accY, float &accZ,
            float &gyroX, float &gyroY, float &gyroZ,
            float &temperatura);
void lerAngulosIMU(float &anguloX, float &anguloY);
void diagnosticarIMU();

#endif
