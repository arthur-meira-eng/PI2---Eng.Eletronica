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
    // Definir os pinos conforme seu esquemático (Ex: GPIOs 34 e 35 que são INPUT ONLY)
    // Se no seu PCB você usou outros, ajuste no ConfigPinos.h
    pinMode(ENCODER_A_PIN, INPUT_PULLUP); 
    pinMode(ENCODER_B_PIN, INPUT_PULLUP);

    // Configura a interrupção para disparar na subida do sinal (RISING)
    attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), interrupcaoEsq, RISING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_B_PIN), interrupcaoDir, RISING);
}

long lerPassosEsq() {
    return contadorEsq;
}

long lerPassosDir() {
    return contadorDir;
}

void resetarEncoders() {
    contadorEsq = 0;
    contadorDir = 0;
}