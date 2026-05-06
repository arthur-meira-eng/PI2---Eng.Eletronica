#include "../include/ToF_VL53L0X.h"
#include "../ConfigPinos.h"

VL53L0X sensor1;
VL53L0X sensor2;

void initToF() {
    pinMode(XSHUT_1, OUTPUT);
    pinMode(XSHUT_2, OUTPUT);

    // Desliga ambos
    digitalWrite(XSHUT_1, LOW);
    digitalWrite(XSHUT_2, LOW);
    delay(10);

    // Liga o primeiro e muda o endereço
    digitalWrite(XSHUT_1, HIGH);
    delay(10);
    sensor1.init();
    sensor1.setAddress(0x30); // Novo endereço para o U25

    // Liga o segundo (mantém o endereço padrão 0x29)
    digitalWrite(XSHUT_2, HIGH);
    delay(10);
    sensor2.init();
    
    sensor1.setTimeout(500);
    sensor2.setTimeout(500);
}

void lerSensoresToF(uint16_t &dist1, uint16_t &dist2) {
    dist1 = sensor1.readRangeSingleMillimeters();
    dist2 = sensor2.readRangeSingleMillimeters();
}