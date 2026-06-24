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

// Macro auxiliar para verificar se algum modo de teste individual está ativo
#define EXECUTANDO_TESTE (MODO_TESTE_RFID || MODO_TESTE_ULTRASSOM || MODO_TESTE_MPU || MODO_TESTE_MOTORES)

// Variáveis para armazenar leituras globais
uint16_t distToF1, distToF2;
float accX, accY, accZ;

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

// =============================================================================
// FUNÇÕES DE OPERAÇÃO INTEGRADA
// =============================================================================

void setupIntegrado() {
    Serial.println("[SETUP] Inicializando modo integrado...");

    // 1. Inicializa Barramento I2C se algum dispositivo I2C estiver ativo
    #if HABILITAR_TOF || HABILITAR_IMU || HABILITAR_ADC
    iniciarI2C();
    #endif

    // 2. Inicializa Atuadores e Sensores de Baixo Nível
    #if HABILITAR_MOTORES
    initMotores();
    #endif

    #if HABILITAR_ENCODERS
    initEncoders();
    #endif

    #if HABILITAR_ULTRASSOM
    initUltrassom();
    #endif

    // 3. Inicializa RFID via SPI
    #if HABILITAR_RFID
    initRFID();
    #endif

    // 4. Inicializa Dispositivos I2C
    #if HABILITAR_TOF
    initToF();
    #endif

    #if HABILITAR_IMU
    initIMU();
    #endif

    #if HABILITAR_ADC
    initADC();
    #endif

    Serial.println("--- SISTEMA INTEGRADO PRONTO PARA OPERAÇÃO ---\n");
}

void loopIntegrado() {
    // --- 1. RFID (INTERRUPÇÃO) ---
    #if HABILITAR_RFID
    String tagRFID = lerTagRFID();
    if (tagRFID.length() > 0) {
        Serial.print("RFID detectado: ");
        Serial.println(tagRFID);
    }
    #endif

    // --- 2. MONITORAMENTO DE SEGURANÇA (HARDWARE) ---
    #if HABILITAR_ADC
    float vBat1 = lerTensaoBateria(1);
    float vBat2 = lerTensaoBateria(2);
    
    if ((vBat1 > 0.1 && vBat1 < 10.5) || (vBat2 > 0.1 && vBat2 < 10.5)) {
        Serial.printf("[SEGURANCA] ALERTA: bateria baixa. BAT1=%.2f V | BAT2=%.2f V. Parando atuadores.\n",
                      vBat1, vBat2);
        #if HABILITAR_MOTORES
        pararTudo(); 
        #endif
    }
    #else
    float vBat1 = 0.0, vBat2 = 0.0;
    #endif

    // --- 3. LEITURA DE SENSORES ---
    #if HABILITAR_TOF
    lerSensoresToF(distToF1, distToF2);
    #else
    distToF1 = 0;
    distToF2 = 0;
    #endif

    #if HABILITAR_ULTRASSOM
    float distUltra = lerDistancia();
    #else
    float distUltra = 0.0;
    #endif

    #if HABILITAR_ADC
    float iM1 = lerCorrenteMotor(1);
    float iM2 = lerCorrenteMotor(2);
    #else
    float iM1 = 0.0, iM2 = 0.0;
    #endif

    // --- 4. DEBUG/TELEMETRIA ---
    #if DEBUG_TELEMETRIA_USB
    static unsigned long tDebug = 0;
    if (millis() - tDebug >= INTERVALO_LOG_SISTEMA_MS) {
        float imuX = 0.0, imuY = 0.0;
        #if HABILITAR_IMU
        lerAngulosIMU(imuX, imuY);
        #endif
        
        long passosEsq = 0, passosDir = 0;
        #if HABILITAR_ENCODERS
        passosEsq = lerPassosEsq();
        passosDir = lerPassosDir();
        #endif

        Serial.printf("[SISTEMA] BAT1=%.2fV BAT2=%.2fV | I_M1=%.2fA I_M2=%.2fA | ToF1=%umm ToF2=%umm Ultra=%.1fcm | ENC_E=%ld ENC_D=%ld | IMU_X=%.2f IMU_Y=%.2f\n",
                      vBat1, vBat2, iM1, iM2, (unsigned)distToF1, (unsigned)distToF2, distUltra,
                      passosEsq, passosDir, imuX, imuY);
        tDebug = millis();
    }
    #endif

    delay(10);
}

// =============================================================================
// FUNÇÕES DE TESTE / DIAGNÓSTICO INDIVIDUAL
// =============================================================================

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

void setupTesteIndividual() {
    #if MODO_TESTE_ULTRASSOM
        Serial.println("MODO TESTE ULTRASSOM: demais sensores e atuadores desabilitados.");
        initUltrassom();
        Serial.println("--- SISTEMA PRONTO PARA TESTE DO ULTRASSOM ---\n");
    #elif MODO_TESTE_MPU
        Serial.println("MODO TESTE MPU6050: demais sensores e atuadores desabilitados.");
        iniciarI2C();
        initIMU();
        Serial.println("--- SISTEMA PRONTO PARA TESTE DO MPU6050 ---\n");
    #elif MODO_TESTE_RFID
        Serial.println("MODO TESTE RFID: sensores I2C e atuadores desabilitados.");
        initRFID();
        Serial.println("Leitor RFID: OK");
        Serial.println("--- SISTEMA PRONTO PARA TESTE RFID ---\n");
    #elif MODO_TESTE_MOTORES
        Serial.println("MODO TESTE MOTORES + ENCODERS: demais sensores e comunicacoes desabilitados.");
        Serial.printf("[TESTE MOTOR+ENC] PWM tracao=%d | PWM carga=%d | etapa=%lums | log encoders=%lums\n",
                      TESTE_MOTOR_PWM, TESTE_MOTOR_CARGA_PWM, (unsigned long)TESTE_MOTOR_ETAPA_MS);
        initMotores();
        initEncoders();
        resetarEncoders();
        Serial.println("[TESTE MOTOR+ENC] Sequencia: esq frente -> dir frente -> ambos frente -> ambos tras -> carga sobe -> carga desce.");
        Serial.println("[TESTE MOTOR+ENC] Observe se as contagens/deltas dos encoders mudam junto com os motores de tracao.");
        Serial.println("--- SISTEMA PRONTO PARA TESTE DOS MOTORES E ENCODERS ---\n");
    #endif
}

void loopTesteIndividual() {
    #if MODO_TESTE_ULTRASSOM
        diagnosticarUltrassom();
        delay(50);
    #elif MODO_TESTE_MPU
        diagnosticarIMU();
        delay(10);
    #elif MODO_TESTE_RFID
        String tagTesteRFID = lerTagRFID();
        if (tagTesteRFID.length() > 0) {
            Serial.print("RFID detectado: ");
            Serial.println(tagTesteRFID);
        }
        delay(10);
    #elif MODO_TESTE_MOTORES
        diagnosticarMotoresEncoders();
        delay(10);
    #endif
}

// =============================================================================
// FUNÇÕES PADRÃO DO ARDUINO (setup e loop)
// =============================================================================

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10); 
    delay(500);
    Serial.println("[SETUP] Serial USB iniciada em 115200 baud.");
    Serial.println("\n--- INICIALIZANDO AGV EMPILHADEIRA ---");
    logMapaPinos();

    #if EXECUTANDO_TESTE
        setupTesteIndividual();
    #else
        setupIntegrado();
    #endif
}

void loop() {
    #if EXECUTANDO_TESTE
        loopTesteIndividual();
    #else
        loopIntegrado();
    #endif
}
