#line 1 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/src/Encoders.cpp"
#include "../include/Encoders.h"
#include "../ConfigPinos.h"

// Variáveis voláteis para serem usadas dentro das interrupções
volatile long contadorEsq = 0;
volatile long contadorDir = 0;
volatile uint8_t estadoAnteriorEsq = 0;
volatile uint8_t estadoAnteriorDir = 0;

static inline uint8_t lerEstadoEncoder(uint8_t pinoA, uint8_t pinoB) {
    return (digitalRead(pinoA) << 1) | digitalRead(pinoB);
}

static inline void atualizarContadorQuadratura(volatile long &contador, volatile uint8_t &estadoAnterior,
                                               uint8_t estadoAtual) {
    const uint8_t transicao = (estadoAnterior << 2) | estadoAtual;

    if (transicao == 0b0001 || transicao == 0b0111 || transicao == 0b1110 || transicao == 0b1000) {
        contador++;
    } else if (transicao == 0b0010 || transicao == 0b1011 || transicao == 0b1101 || transicao == 0b0100) {
        contador--;
    }

    estadoAnterior = estadoAtual;
}

// Funções de interrupção (ISR) - executadas em microssegundos
void IRAM_ATTR interrupcaoEsq() {
    atualizarContadorQuadratura(contadorEsq, estadoAnteriorEsq,
                                lerEstadoEncoder(ENCODER_ESQ_A_PIN, ENCODER_ESQ_B_PIN));
}

void IRAM_ATTR interrupcaoDir() {
    atualizarContadorQuadratura(contadorDir, estadoAnteriorDir,
                                lerEstadoEncoder(ENCODER_DIR_A_PIN, ENCODER_DIR_B_PIN));
}

void initEncoders() {
    Serial.println("[ENCODERS] Inicializando entradas com interrupcao...");
    Serial.printf("[ENCODERS] Esq A=GPIO %d B=GPIO %d | Dir A=GPIO %d B=GPIO %d\n",
                  ENCODER_ESQ_A_PIN, ENCODER_ESQ_B_PIN,
                  ENCODER_DIR_A_PIN, ENCODER_DIR_B_PIN);
    // GPIOs 34 a 39 sao somente entrada e nao oferecem pull-up interno confiavel.
    // O condicionamento eletrico deve ficar no encoder/PCB.
    pinMode(ENCODER_ESQ_A_PIN, INPUT);
    pinMode(ENCODER_ESQ_B_PIN, INPUT);
    pinMode(ENCODER_DIR_A_PIN, INPUT);
    pinMode(ENCODER_DIR_B_PIN, INPUT);

    estadoAnteriorEsq = lerEstadoEncoder(ENCODER_ESQ_A_PIN, ENCODER_ESQ_B_PIN);
    estadoAnteriorDir = lerEstadoEncoder(ENCODER_DIR_A_PIN, ENCODER_DIR_B_PIN);

    attachInterrupt(digitalPinToInterrupt(ENCODER_ESQ_A_PIN), interrupcaoEsq, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_ESQ_B_PIN), interrupcaoEsq, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_DIR_A_PIN), interrupcaoDir, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENCODER_DIR_B_PIN), interrupcaoDir, CHANGE);
    Serial.println("[ENCODERS] Interrupcoes anexadas.");
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
    estadoAnteriorEsq = lerEstadoEncoder(ENCODER_ESQ_A_PIN, ENCODER_ESQ_B_PIN);
    estadoAnteriorDir = lerEstadoEncoder(ENCODER_DIR_A_PIN, ENCODER_DIR_B_PIN);
    interrupts();
}
