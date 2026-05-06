#include "../include/Ultrassom_HCSR04.h"
#include "../ConfigPinos.h"

void initUltrassom() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

float lerDistancia() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duracao = pulseIn(ECHO_PIN, HIGH, 30000); // timeout de 30ms
    float distancia = (duracao * 0.0343) / 2;
    return distancia;
}