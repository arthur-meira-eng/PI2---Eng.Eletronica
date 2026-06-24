#line 1 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/src/Motores.cpp"
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

static void acionarMotor(int pinPWM, int pinDIR, int velocidade, bool invertido) {
    velocidade = constrain(velocidade, -255, 255);
    bool sentidoFrente = velocidade >= 0;
    if (invertido) {
        sentidoFrente = !sentidoFrente;
    }
    digitalWrite(pinDIR, sentidoFrente ? HIGH : LOW);
    analogWrite(pinPWM, abs(velocidade));
}

void acionarMotorEsq(int velocidade) {
    acionarMotor(PWM_ESQ, DIR_ESQ, velocidade, MOTOR_ESQ_INVERTIDO);
}

void acionarMotorDir(int velocidade) {
    acionarMotor(PWM_DIR, DIR_DIR, velocidade, MOTOR_DIR_INVERTIDO);
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
    digitalWrite(PWM_ESQ, LOW);
    digitalWrite(PWM_DIR, LOW);
    digitalWrite(PWM_CARG, LOW);
    digitalWrite(DIR_ESQ, LOW);
    digitalWrite(DIR_DIR, LOW);
    digitalWrite(DIR_CARG, LOW);
}
