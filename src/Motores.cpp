#include "../include/Motores.h"
#include "../ConfigPinos.h"

void initMotores() {
    pinMode(PWM_ESQ, OUTPUT);
    pinMode(DIR_ESQ, OUTPUT);
    pinMode(PWM_DIR, OUTPUT);
    pinMode(DIR_DIR, OUTPUT);
    pinMode(PWM_CARG, OUTPUT);
    pinMode(DIR_CARG, OUTPUT);
    
    pararTudo();
}

void acionarMotorEsq(int velocidade) {
    digitalWrite(DIR_ESQ, velocidade >= 0 ? HIGH : LOW);
    analogWrite(PWM_ESQ, abs(velocidade));
}

void acionarMotorDir(int velocidade) {
    digitalWrite(DIR_DIR, velocidade >= 0 ? HIGH : LOW);
    analogWrite(PWM_DIR, abs(velocidade));
}

void acionarMotorCarga(int velocidade, bool subir) {
    digitalWrite(DIR_CARG, subir ? HIGH : LOW);
    analogWrite(PWM_CARG, velocidade);
}

void pararTudo() {
    analogWrite(PWM_ESQ, 0);
    analogWrite(PWM_DIR, 0);
    analogWrite(PWM_CARG, 0);
}