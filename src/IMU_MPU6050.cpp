#include "../include/IMU_MPU6050.h"

Adafruit_MPU6050 mpu;

void initIMU() {
    if (!mpu.begin()) {
        Serial.println("Falha ao encontrar MPU6050");
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
}

void lerAceleracao(float &x, float &y, float &z) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    x = a.acceleration.x;
    y = a.acceleration.y;
    z = a.acceleration.z;
}