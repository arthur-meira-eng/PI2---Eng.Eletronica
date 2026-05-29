#include "../include/Motores.h"
#include "../ConfigPinos.h"

void initMotores() {
    Serial.println("[MOTORES] Inicializando ponte/driver dos motores...");
    Serial.printf("[MOTORES] Esq PWM=GPIO %d DIR=GPIO %d | Dir PWM=GPIO %d DIR=GPIO %d | Carga PWM=GPIO %d DIR=GPIO %d\n",
                  PWM_ESQ, DIR_ESQ, PWM_DIR, DIR_DIR, PWM_CARG, DIR_CARG);
    pinMode(PWM_ESQ, OUTPUT);
    pinMode(DIR_ESQ, OUTPUT);
    pinMode(PWM_DIR, OUTPUT);
    pinMode(DIR_DIR, OUTPUT);
    pinMode(PWM_CARG, OUTPUT);
    pinMode(DIR_CARG, OUTPUT);
    
    pararTudo();
    Serial.println("[MOTORES] Saidas configuradas e todos os motores parados.");
}

void acionarMotorEsq(int velocidade) {
    velocidade = constrain(velocidade, -255, 255);
    digitalWrite(DIR_ESQ, velocidade >= 0 ? HIGH : LOW);
    analogWrite(PWM_ESQ, abs(velocidade));
}

void acionarMotorDir(int velocidade) {
    velocidade = constrain(velocidade, -255, 255);
    digitalWrite(DIR_DIR, velocidade >= 0 ? HIGH : LOW);
    analogWrite(PWM_DIR, abs(velocidade));
}

void acionarMotorCarga(int velocidade, bool subir) {
    velocidade = constrain(velocidade, 0, 255);
    digitalWrite(DIR_CARG, subir ? HIGH : LOW);
    analogWrite(PWM_CARG, velocidade);
}

void pararTudo() {
    analogWrite(PWM_ESQ, 0);
    analogWrite(PWM_DIR, 0);
    analogWrite(PWM_CARG, 0);
}
