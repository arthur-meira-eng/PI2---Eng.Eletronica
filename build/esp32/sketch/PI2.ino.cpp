#include <Arduino.h>
#line 1 "/home/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
/**
 * PROJETO: AGV Empilhadeira - PI2 (UnB FGA)
 * AUTOR: Arthur Henrique / Karen Julia
 * DESCRIÇÃO: Arquivo principal que integra os drivers de hardware e a UART.
 */

#include "ConfigPinos.h"
#include "include/Motores.h"
#include "include/Encoders.h"
#include "include/ToF_VL53L0X.h"
#include "include/IMU_MPU6050.h"
#include "include/RFID_RC522.h"
#include "include/Ultrassom_HCSR04.h"
#include "include/ADC_ADS1115.h"
#include "include/ComunicacaoRasp.h" // Adicionado para a UART

// Variáveis para armazenar leituras globais
uint16_t distToF1, distToF2;
float accX, accY, accZ;

#line 21 "/home/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void setup();
#line 70 "/home/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void loop();
#line 21 "/home/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void setup() {
    // 1. Inicializa Comunicação Serial para Debug (USB)
    Serial.begin(115200);
    while (!Serial) delay(10); 
    Serial.println("\n--- INICIALIZANDO AGV EMPILHADEIRA ---");

#if MODO_TESTE_RFID
    Serial.println("MODO TESTE RFID: sensores I2C e atuadores desabilitados.");
    initRFID();
    Serial.println("Leitor RFID: OK");
    Serial.println("--- SISTEMA PRONTO PARA TESTE RFID ---\n");
    return;
#endif

    // 2. Inicializa Barramento I2C (Compartilhado por ToF, MPU e ADC)
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.println("Barramento I2C: OK");

    // 3. Inicializa Atuadores e Sensores de Baixo Nível
    initMotores();
    Serial.println("Motores: OK");

    initEncoders();
    Serial.println("Encoders: OK");

    initUltrassom();
    Serial.println("Ultrassom: OK");

    // 4. Inicializa RFID via SPI cedo para facilitar debug de tag
    initRFID();     // RC522 via SPI
    Serial.println("Leitor RFID: OK");

    // 5. Inicializa Dispositivos I2C
    initToF();      // Gerencia os dois VL53L0X via XSHUT
    Serial.println("Sensores ToF: OK");

    initIMU();      // MPU6050
    Serial.println("Inercial MPU6050: OK");

    initADC();      // ADS1115 para Bateria e Corrente
    Serial.println("ADC ADS1115: OK");

    // 6. Inicializa Comunicação com a Raspberry Pi (UART2)
    initComunicacao(); 
    Serial.println("Comunicacao Raspberry: OK");

    Serial.println("--- SISTEMA PRONTO PARA OPERAÇÃO ---\n");
}

void loop() {
#if MODO_TESTE_RFID
    String tagTesteRFID = lerTagRFID();
    if (tagTesteRFID.length() > 0) {
        Serial.print("RFID detectado: ");
        Serial.println(tagTesteRFID);
    }
    delay(10);
    return;
#endif

    // --- 1. COMUNICAÇÃO ---
    // Escuta comandos vindos da Raspberry (ex: "PARAR")
    lerComandos();

    // --- 2. RFID (INTERRUPÇÃO) ---
    String tagRFID = lerTagRFID();
    if (tagRFID.length() > 0) {
        Serial.print("RFID detectado: ");
        Serial.println(tagRFID);
        Serial2.print("RFID:");
        Serial2.println(tagRFID);
    }

    // --- 3. MONITORAMENTO DE SEGURANÇA (HARDWARE) ---
    float vBat1 = lerTensaoBateria(1);
    float vBat2 = lerTensaoBateria(2);
    
    if (vBat1 < 10.5 || vBat2 < 10.5) {
        Serial.print("ALERTA: Bateria Baixa! BAT1: "); Serial.print(vBat1);
        Serial.print("V | BAT2: "); Serial.println(vBat2);
        pararTudo(); 
    }

    // --- 4. LEITURA DE SENSORES ---
    lerSensoresToF(distToF1, distToF2);
    float distUltra = lerDistancia();
    float iM1 = lerCorrenteMotor(1);
    float iM2 = lerCorrenteMotor(2);

    // --- 5. TELEMETRIA PARA RASPBERRY ---
    // Envia dados a cada 500ms para não sobrecarregar a serial
    static unsigned long tTele = 0;
    if (millis() - tTele > 500) {
        enviarDados(); 
        tTele = millis();
    }

    // --- 6. DEBUG (OPCIONAL) ---
    /*
    Serial.printf("Distâncias -> ToF1: %dmm | ToF2: %dmm | Ultra: %.2fcm\n", distToF1, distToF2, distUltra);
    Serial.printf("Encoders -> Esq: %ld | Dir: %ld\n", lerPassosEsq(), lerPassosDir());
    */

    delay(10); // Loop rápido para garantir resposta imediata aos comandos UART
}

