#include "../include/ToF_VL53L0X.h"
#include "../ConfigPinos.h"

VL53L0X sensor1;
VL53L0X sensor2;
static bool tof1Ok = false;
static bool tof2Ok = false;

void initToF() {
    Serial.println("[TOF] Inicializando dois VL53L0X no I2C...");
    Serial.printf("[TOF] XSHUT_1=GPIO %d -> endereco 0x30 | XSHUT_2=GPIO %d -> endereco 0x29\n",
                  XSHUT_1, XSHUT_2);
    pinMode(XSHUT_1, OUTPUT);
    pinMode(XSHUT_2, OUTPUT);

    // Desliga ambos
    digitalWrite(XSHUT_1, LOW);
    digitalWrite(XSHUT_2, LOW);
    delay(10);

    // Liga o primeiro e muda o endereço
    digitalWrite(XSHUT_1, HIGH);
    delay(10);
    tof1Ok = sensor1.init();
    if (tof1Ok) {
        sensor1.setAddress(0x30); // Novo endereço para o U25
        Serial.println("[TOF] Sensor 1 detectado e remapeado para 0x30.");
    } else {
        Serial.println("[TOF] ERRO: Sensor 1 nao respondeu no endereco padrao 0x29.");
    }

    // Liga o segundo (mantém o endereço padrão 0x29)
    digitalWrite(XSHUT_2, HIGH);
    delay(10);
    tof2Ok = sensor2.init();
    if (tof2Ok) {
        Serial.println("[TOF] Sensor 2 detectado no endereco 0x29.");
    } else {
        Serial.println("[TOF] ERRO: Sensor 2 nao respondeu no endereco padrao 0x29.");
    }
    
    sensor1.setTimeout(500);
    sensor2.setTimeout(500);
    Serial.printf("[TOF] Status final: sensor1=%s | sensor2=%s\n",
                  tof1Ok ? "OK" : "FALHA", tof2Ok ? "OK" : "FALHA");
}

void lerSensoresToF(uint16_t &dist1, uint16_t &dist2) {
    dist1 = tof1Ok ? sensor1.readRangeSingleMillimeters() : 0;
    dist2 = tof2Ok ? sensor2.readRangeSingleMillimeters() : 0;

    static unsigned long ultimoLogErro = 0;
    if (millis() - ultimoLogErro >= 2000) {
        if (tof1Ok && sensor1.timeoutOccurred()) {
            Serial.println("[TOF] ALERTA: timeout na leitura do sensor 1.");
        }
        if (tof2Ok && sensor2.timeoutOccurred()) {
            Serial.println("[TOF] ALERTA: timeout na leitura do sensor 2.");
        }
        ultimoLogErro = millis();
    }
}
