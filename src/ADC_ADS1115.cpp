#include "../include/ADC_ADS1115.h"

Adafruit_ADS1115 ads;

// Fatores de Calibração
// Se o seu divisor de tensão for, por exemplo, 10k e 2.2k, o fator é ~5.54
const float FATOR_BAT1 = 4.0; 
const float FATOR_BAT2 = 4.0; 
const float SENSIBILIDADE_CORRENTE = 0.100; // 100mV por Ampère

void initADC() {
    if (!ads.begin()) {
        Serial.println("Erro: ADS1115 não encontrado!");
    }
    ads.setGain(GAIN_ONE); // Faixa de +/- 4.096V
}

// Retorna a tensão da bateria escolhida (1 ou 2)
float lerTensaoBateria(int bateria) {
    int16_t leitura;
    if (bateria == 1) {
        leitura = ads.readADC_SingleEnded(2); // BAT1 está no A2
        return ads.computeVolts(leitura) * FATOR_BAT1;
    } else {
        leitura = ads.readADC_SingleEnded(3); // BAT2 está no A3
        return ads.computeVolts(leitura) * FATOR_BAT2;
    }
}

// Retorna a corrente do motor escolhido (1 ou 2)
float lerCorrenteMotor(int motor) {
    int16_t leitura;
    if (motor == 1) {
        leitura = ads.readADC_SingleEnded(0); // M1_I está no A0
    } else {
        leitura = ads.readADC_SingleEnded(1); // M2_I está no A1
    }
    
    float voltagem = ads.computeVolts(leitura);
    // Ajuste o cálculo abaixo conforme o seu sensor de corrente (ex: ACS712)
    return (voltagem - 2.5) / SENSIBILIDADE_CORRENTE; 
}