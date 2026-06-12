#include <Arduino.h>
#line 1 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
/**
 * PROJETO: AGV Empilhadeira - PI2 (UnB FGA)
 * AUTOR: Arthur Henrique / Karen Julia
 * DESCRIÇÃO: Arquivo principal que integra os drivers de hardware.
 */

#include "ConfigPinos.h"
#include "include/Motores.h"
#include "include/Encoders.h"
#include "include/ToF_VL53L0X.h"
#include "include/IMU_MPU6050.h"
#include "include/RFID_RC522.h"
#include "include/Ultrassom_HCSR04.h"
#include "include/ADC_ADS1115.h"

// Variáveis para armazenar leituras globais
uint16_t distToF1, distToF2;
float accX, accY, accZ;

#line 20 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void logMapaPinos();
#line 33 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void iniciarI2C();
#line 38 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void diagnosticarMotoresEncoders();
#line 114 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void setup();
#line 182 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void loop();
#line 20 "/Users/yaba/Sandbox/PI2---Eng.Eletronica/PI2/PI2.ino"
void logMapaPinos() {
    Serial.println("[PINOS] Mapa integrado configurado:");
    Serial.printf("[PINOS] Motores -> Esq PWM=%d DIR=%d | Dir PWM=%d DIR=%d | Carga PWM=%d DIR=%d\n",
                  PWM_ESQ, DIR_ESQ, PWM_DIR, DIR_DIR, PWM_CARG, DIR_CARG);
    Serial.printf("[PINOS] I2C -> SDA=%d SCL=%d | ToF XSHUT1=%d XSHUT2=%d\n",
                  I2C_SDA, I2C_SCL, XSHUT_1, XSHUT_2);
    Serial.printf("[PINOS] RFID/SPI -> SCK=%d MISO=%d MOSI=%d SS=%d RST=%d IRQ=%d\n",
                  SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SS_PIN, RST_PIN, IRQ_PIN);
    Serial.printf("[PINOS] Ultrassom -> TRIG=%d ECHO=%d\n", TRIG_PIN, ECHO_PIN);
    Serial.printf("[PINOS] Encoders -> Esq A=%d B=%d | Dir A=%d B=%d\n",
                  ENCODER_ESQ_A_PIN, ENCODER_ESQ_B_PIN, ENCODER_DIR_A_PIN, ENCODER_DIR_B_PIN);
}

void iniciarI2C() {
    Wire.begin(I2C_SDA, I2C_SCL);
    Serial.printf("[I2C] Barramento iniciado: SDA=GPIO %d | SCL=GPIO %d\n", I2C_SDA, I2C_SCL);
}

void diagnosticarMotoresEncoders() {
    struct EtapaMotor {
        const char *nome;
        int velEsq;
        int velDir;
        int velCarga;
        bool cargaSobe;
        bool resetarContagem;
    };

    static const EtapaMotor etapas[] = {
        {"PARADO", 0, 0, 0, true, false},
        {"AMBOS FRENTE", TESTE_MOTOR_PWM, TESTE_MOTOR_PWM, 0, true, true},
        // {"PARADO", 0, 0, 0, true, false},
        // {"AMBOS TRAS", -TESTE_MOTOR_PWM, -TESTE_MOTOR_PWM, 0, true, true},
        // {"PARADO", 0, 0, 0, true, false},
    };

    static uint8_t etapaAtual = 0;
    static unsigned long inicioEtapa = 0;
    static unsigned long ultimoLog = 0;
    static bool primeiraExecucao = true;
    static long passosEsqAnterior = 0;
    static long passosDirAnterior = 0;

    if (primeiraExecucao || millis() - inicioEtapa >= TESTE_MOTOR_ETAPA_MS) {
        primeiraExecucao = false;
        inicioEtapa = millis();
        ultimoLog = 0;

        const EtapaMotor &etapa = etapas[etapaAtual];
        pararTudo();
        delay(100);

        if (etapa.velEsq != 0) {
            acionarMotorEsq(etapa.velEsq);
        }

        if (etapa.velDir != 0) {
            acionarMotorDir(etapa.velDir);
        }

        if (etapa.velCarga != 0) {
            acionarMotorCarga(etapa.velCarga, etapa.cargaSobe);
        }

        if (etapa.resetarContagem) {
            resetarEncoders();
        }

        passosEsqAnterior = lerPassosEsq();
        passosDirAnterior = lerPassosDir();

        Serial.printf("[TESTE MOTOR+ENC] %-15s | Esq=%4d Dir=%4d Carga=%3d %s | ENC_E=%ld ENC_D=%ld | t=%lums\n",
                      etapa.nome, etapa.velEsq, etapa.velDir, etapa.velCarga,
                      etapa.cargaSobe ? "SOBE" : "DESCE",
                      passosEsqAnterior, passosDirAnterior, millis());

        etapaAtual = (etapaAtual + 1) % (sizeof(etapas) / sizeof(etapas[0]));
    }

    if (millis() - ultimoLog >= INTERVALO_LOG_SISTEMA_MS) {
        const long passosEsq = lerPassosEsq();
        const long passosDir = lerPassosDir();

        Serial.printf("[ENCODERS] Esq=%ld d=%+ld | Dir=%ld d=%+ld | etapa ha %lums\n",
                      passosEsq, passosEsq - passosEsqAnterior,
                      passosDir, passosDir - passosDirAnterior,
                      millis() - inicioEtapa);

        passosEsqAnterior = passosEsq;
        passosDirAnterior = passosDir;
        ultimoLog = millis();
    }
}

void setup() {
    // 1. Inicializa Comunicação Serial para Debug (USB)
    Serial.begin(115200);
    while (!Serial) delay(10); 
    delay(500);
    Serial.println("[SETUP] Serial USB iniciada em 115200 baud.");
    Serial.println("\n--- INICIALIZANDO AGV EMPILHADEIRA ---");
    logMapaPinos();

#if MODO_TESTE_ULTRASSOM
    Serial.println("MODO TESTE ULTRASSOM: demais sensores e atuadores desabilitados.");
    initUltrassom();
    Serial.println("--- SISTEMA PRONTO PARA TESTE DO ULTRASSOM ---\n");
    return;
#endif

#if MODO_TESTE_MPU
    Serial.println("MODO TESTE MPU6050: demais sensores e atuadores desabilitados.");
    iniciarI2C();
    initIMU();
    Serial.println("--- SISTEMA PRONTO PARA TESTE DO MPU6050 ---\n");
    return;
#endif

#if MODO_TESTE_RFID
    Serial.println("MODO TESTE RFID: sensores I2C e atuadores desabilitados.");
    initRFID();
    Serial.println("Leitor RFID: OK");
    Serial.println("--- SISTEMA PRONTO PARA TESTE RFID ---\n");
    return;
#endif

#if MODO_TESTE_MOTORES
    Serial.println("MODO TESTE MOTORES + ENCODERS: demais sensores e comunicacoes desabilitados.");
    Serial.printf("[TESTE MOTOR+ENC] PWM tracao=%d | PWM carga=%d | etapa=%lums | log encoders=%lums\n",
                  TESTE_MOTOR_PWM, TESTE_MOTOR_CARGA_PWM, (unsigned long)TESTE_MOTOR_ETAPA_MS);
    initMotores();
    initEncoders();
    resetarEncoders();
    Serial.println("[TESTE MOTOR+ENC] Sequencia: esq frente -> dir frente -> ambos frente -> ambos tras -> carga sobe -> carga desce.");
    Serial.println("[TESTE MOTOR+ENC] Observe se as contagens/deltas dos encoders mudam junto com os motores de tracao.");
    Serial.println("--- SISTEMA PRONTO PARA TESTE DOS MOTORES E ENCODERS ---\n");
    return;
#endif

    // 2. Inicializa Barramento I2C (Compartilhado por ToF, MPU e ADC)
    iniciarI2C();

    // 3. Inicializa Atuadores e Sensores de Baixo Nível
    initMotores();

    initEncoders();

    initUltrassom();

    // 4. Inicializa RFID via SPI cedo para facilitar debug de tag
    initRFID();     // RC522 via SPI

    // 5. Inicializa Dispositivos I2C
    initToF();      // Gerencia os dois VL53L0X via XSHUT

    initIMU();      // MPU6050

    initADC();      // ADS1115 para Bateria e Corrente

    Serial.println("--- SISTEMA PRONTO PARA OPERAÇÃO ---\n");
}

void loop() {
#if MODO_TESTE_ULTRASSOM
    diagnosticarUltrassom();
    delay(50);
    return;
#endif

#if MODO_TESTE_MPU
    diagnosticarIMU();
    delay(10);
    return;
#endif

#if MODO_TESTE_RFID
    String tagTesteRFID = lerTagRFID();
    if (tagTesteRFID.length() > 0) {
        Serial.print("RFID detectado: ");
        Serial.println(tagTesteRFID);
    }
    delay(10);
    return;
#endif

#if MODO_TESTE_MOTORES
    diagnosticarMotoresEncoders();
    delay(10);
    return;
#endif

    // --- 1. RFID (INTERRUPÇÃO) ---
    String tagRFID = lerTagRFID();
    if (tagRFID.length() > 0) {
        Serial.print("RFID detectado: ");
        Serial.println(tagRFID);
    }

    // --- 2. MONITORAMENTO DE SEGURANÇA (HARDWARE) ---
    float vBat1 = lerTensaoBateria(1);
    float vBat2 = lerTensaoBateria(2);
    
    if ((vBat1 > 0.1 && vBat1 < 10.5) || (vBat2 > 0.1 && vBat2 < 10.5)) {
        Serial.printf("[SEGURANCA] ALERTA: bateria baixa. BAT1=%.2f V | BAT2=%.2f V. Parando atuadores.\n",
                      vBat1, vBat2);
        pararTudo(); 
    }

    // --- 3. LEITURA DE SENSORES ---
    lerSensoresToF(distToF1, distToF2);
    float distUltra = lerDistancia();
    float iM1 = lerCorrenteMotor(1);
    float iM2 = lerCorrenteMotor(2);

#if DEBUG_TELEMETRIA_USB
    static unsigned long tDebug = 0;
    if (millis() - tDebug >= INTERVALO_LOG_SISTEMA_MS) {
        float imuX, imuY;
        lerAngulosIMU(imuX, imuY);
        Serial.printf("[SISTEMA] BAT1=%.2fV BAT2=%.2fV | I_M1=%.2fA I_M2=%.2fA | ToF1=%umm ToF2=%umm Ultra=%.1fcm | ENC_E=%ld ENC_D=%ld | IMU_X=%.2f IMU_Y=%.2f\n",
                      vBat1, vBat2, iM1, iM2, (unsigned)distToF1, (unsigned)distToF2, distUltra,
                      lerPassosEsq(), lerPassosDir(), imuX, imuY);
        tDebug = millis();
    }
#endif

    delay(10);
}

