#line 1 "/home/yaba/Sandbox/PI2---Eng.Eletronica/PI2/src/Encoders.cpp"
#include "../include/Encoders.h"
#include "../ConfigPinos.h"

// Variáveis voláteis para serem usadas dentro das interrupções
volatile long contadorEsq = 0;
volatile long contadorDir = 0;

// Funções de interrupção (ISR) - executadas em microssegundos
void IRAM_ATTR interrupcaoEsq() {
    contadorEsq++;
}

void IRAM_ATTR interrupcaoDir() {
    contadorDir++;
}

void initEncoders() {
    // GPIOs 34 e 35 sao somente entrada e nao oferecem pull-up interno confiavel.
    // O condicionamento eletrico deve ficar no encoder/PCB.
    pinMode(ENCODER_ESQ_PIN, INPUT);
    pinMode(ENCODER_DIR_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(ENCODER_ESQ_PIN), interrupcaoEsq, ENCODER_INTERRUPCAO);
    attachInterrupt(digitalPinToInterrupt(ENCODER_DIR_PIN), interrupcaoDir, ENCODER_INTERRUPCAO);
}

long lerPassosEsq() {
    noInterrupts();
    long passos = contadorEsq;
    interrupts();
    return passos;
}

long lerPassosDir() {
    noInterrupts();
    long passos = contadorDir;
    interrupts();
    return passos;
}

void resetarEncoders() {
    noInterrupts();
    contadorEsq = 0;
    contadorDir = 0;
    interrupts();
}
