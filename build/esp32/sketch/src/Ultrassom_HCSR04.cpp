#line 1 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/src/Ultrassom_HCSR04.cpp"
#include "../include/Ultrassom_HCSR04.h"
#include "../ConfigPinos.h"

void initUltrassom() {
    Serial.println("[ULTRASSOM] Inicializando HC-SR04...");
    Serial.printf("[ULTRASSOM] TRIG=GPIO %d | ECHO=GPIO %d\n", TRIG_PIN, ECHO_PIN);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    digitalWrite(TRIG_PIN, LOW);
    Serial.println("[ULTRASSOM] Pinos configurados.");
}

float lerDistancia() {
    int echoAntes = digitalRead(ECHO_PIN);

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duracao = pulseIn(ECHO_PIN, HIGH, 30000); // timeout de 30ms
    float distancia = (duracao * 0.0343) / 2;
    int echoDepois = digitalRead(ECHO_PIN);

    static unsigned long ultimoDebug = 0;
    if (millis() - ultimoDebug >= 500) {
        if (duracao == 0) {
            Serial.printf("[ULTRASSOM] Sem eco recebido. ECHO antes=%d | depois=%d\n", echoAntes, echoDepois);
        } else {
            Serial.printf("[ULTRASSOM] ECHO antes=%d | depois=%d | Duracao: %ld us | Distancia: %.2f cm\n",
                          echoAntes, echoDepois, duracao, distancia);
        }
        ultimoDebug = millis();
    }

    return distancia;
}

void diagnosticarUltrassom() {
    static unsigned long ultimoDiagnostico = 0;
    if (millis() - ultimoDiagnostico >= 1000) {
        Serial.printf("[ULTRASSOM TESTE] Loop ativo. TRIG=GPIO %d | ECHO=GPIO %d | ECHO agora=%d\n",
                      TRIG_PIN, ECHO_PIN, digitalRead(ECHO_PIN));
        ultimoDiagnostico = millis();
    }

    lerDistancia();
}
