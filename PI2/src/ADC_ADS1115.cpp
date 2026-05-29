#include "../include/ADC_ADS1115.h"

Adafruit_ADS1115 ads;
static bool adcOk = false;

// Fatores de Calibração
// Se o seu divisor de tensão for, por exemplo, 10k e 2.2k, o fator é ~5.54
const float FATOR_BAT1 = 4.0; 
const float FATOR_BAT2 = 4.0; 
const float SENSIBILIDADE_CORRENTE = 0.100; // 100mV por Ampère

void initADC() {
    Serial.println("[ADC] Inicializando ADS1115 no barramento I2C...");
    Serial.println("[ADC] Endereco padrao esperado: 0x48 | A0/A1=corrente | A2/A3=baterias");
    if (!ads.begin()) {
        adcOk = false;
        Serial.println("[ADC] ERRO: ADS1115 nao encontrado. Leituras retornarao 0 ate corrigir I2C/alimentacao.");
        return;
    }
    adcOk = true;
    ads.setGain(GAIN_ONE); // Faixa de +/- 4.096V
    Serial.printf("[ADC] ADS1115 pronto. Gain=GAIN_ONE (+/-4.096V) | Fator BAT1=%.2f | BAT2=%.2f | Sens corrente=%.3f V/A\n",
                  FATOR_BAT1, FATOR_BAT2, SENSIBILIDADE_CORRENTE);
}

// Retorna a tensão da bateria escolhida (1 ou 2)
float lerTensaoBateria(int bateria) {
    if (!adcOk) {
        return 0.0;
    }

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
    if (!adcOk) {
        return 0.0;
    }

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
