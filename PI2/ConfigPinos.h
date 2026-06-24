#ifndef CONFIG_PINOS_H
#define CONFIG_PINOS_H

// =============================================================================
// MODOS DE TESTE / DIAGNÓSTICO INDIVIDUAL
// =============================================================================
// Se algum destes for definido como 1, o firmware entra no modo de teste isolado
// daquele componente, desativando todo o resto.
// Se todos forem 0, o firmware roda no modo de operação integrado.
#define MODO_TESTE_RFID 0
#define MODO_TESTE_ULTRASSOM 0
#define MODO_TESTE_MPU 0
#define MODO_TESTE_MOTORES 1

#define TESTE_MOTOR_PWM 120
#define TESTE_MOTOR_CARGA_PWM 100
#define TESTE_MOTOR_ETAPA_MS 10000

// =============================================================================
// ATIVAÇÃO DE COMPONENTES (Para Operação Integrada)
// =============================================================================
// Ative (1) ou desative (0) os componentes para o funcionamento integrado.
#define HABILITAR_MOTORES    1
#define HABILITAR_ENCODERS   1
#define HABILITAR_ULTRASSOM  1
#define HABILITAR_RFID       1
#define HABILITAR_TOF        1
#define HABILITAR_IMU        1
#define HABILITAR_ADC        1

// Configurações de logs e telemetria
#define DEBUG_TELEMETRIA_USB 1
#define INTERVALO_LOG_SISTEMA_MS 1000

// --- Motores (Etiquetas do Esquema) ---
// Os pinos "DIR" definem o sentido lógico (HIGH = frente, LOW = trás, ou vice-versa).

// Motor Esquerdo
#define PWM_ESQ   32 // Pino de controle de velocidade (PWM)
#define DIR_ESQ   33 // Pino de sentido de rotação lógico

// Motor Direito
#define PWM_DIR   25 // Pino de controle de velocidade (PWM)
#define DIR_DIR   26 // Pino de sentido de rotação lógico

// Motor de Carga (Elevação/Descarga)
#define PWM_CARG  27 // Pino de controle de velocidade (PWM)
#define DIR_CARG  14 // Pino de sentido de rotação lógico

// Ajuste de sentido dos motores de tracao.
// Use 1 quando "frente" no software estiver fazendo o motor girar para tras.
#define MOTOR_ESQ_INVERTIDO 1
#define MOTOR_DIR_INVERTIDO 1

// --- Sensores I2C (SDA=21, SCL=22) ---
#define I2C_SDA   21
#define I2C_SCL   22

#define XSHUT_1   4  // VL53L0X U25
#define XSHUT_2   2  // VL53L0X U26

// SENSOR IR
#define IR_PIN 15

// --- Sensor Ultrassônico HC-SR04 ---
// GPIO 18 e GPIO 5 ficam reservados ao SPI do RC522 (SCK e SS).
#define TRIG_PIN  12
#define ECHO_PIN  13  // entrada somente; use divisor de tensao no ECHO do HC-SR04

// --- RFID RC522 (SPI) ---
#define SPI_SCK_PIN   18
#define SPI_MISO_PIN  19
#define SPI_MOSI_PIN  23
#define SS_PIN        5

#if MODO_TESTE_RFID
    #define RST_PIN   22
#else
    #define RST_PIN   17
#endif

#define IRQ_PIN   4
#define RFID_USAR_IRQ 1

// --- Encoders ---
#define ENCODER_ESQ_A_PIN  39  // CN10 - requer pull-up/pull-down externo no ESP32
#define ENCODER_DIR_A_PIN  36  // CN11 - requer pull-up/pull-down externo no ESP32

#define ENCODER_ESQ_B_PIN  35  // CN11 - requer pull-up/pull-down externo no ESP32
#define ENCODER_DIR_B_PIN  34  // CN11 - requer pull-up/pull-down externo no ESP32

#define ENCODER_INTERRUPCAO RISING

#define ASSERT_PIN_DIFF(A, B) static_assert((A) != (B), "Conflito de GPIO entre " #A " e " #B ".")

#if !MODO_TESTE_RFID
ASSERT_PIN_DIFF(RST_PIN, I2C_SCL);
ASSERT_PIN_DIFF(RST_PIN, TRIG_PIN);
ASSERT_PIN_DIFF(RST_PIN, ECHO_PIN);
#endif

ASSERT_PIN_DIFF(PWM_ESQ, DIR_ESQ);
ASSERT_PIN_DIFF(PWM_ESQ, PWM_DIR);
ASSERT_PIN_DIFF(PWM_ESQ, DIR_DIR);
ASSERT_PIN_DIFF(PWM_ESQ, PWM_CARG);
ASSERT_PIN_DIFF(PWM_ESQ, DIR_CARG);
ASSERT_PIN_DIFF(DIR_ESQ, PWM_DIR);
ASSERT_PIN_DIFF(DIR_ESQ, DIR_DIR);
ASSERT_PIN_DIFF(DIR_ESQ, PWM_CARG);
ASSERT_PIN_DIFF(DIR_ESQ, DIR_CARG);
ASSERT_PIN_DIFF(PWM_DIR, DIR_DIR);
ASSERT_PIN_DIFF(PWM_DIR, PWM_CARG);
ASSERT_PIN_DIFF(PWM_DIR, DIR_CARG);
ASSERT_PIN_DIFF(DIR_DIR, PWM_CARG);
ASSERT_PIN_DIFF(DIR_DIR, DIR_CARG);
ASSERT_PIN_DIFF(PWM_CARG, DIR_CARG);

ASSERT_PIN_DIFF(I2C_SDA, I2C_SCL);
ASSERT_PIN_DIFF(I2C_SDA, XSHUT_1);
ASSERT_PIN_DIFF(I2C_SDA, XSHUT_2);
ASSERT_PIN_DIFF(I2C_SCL, XSHUT_1);
ASSERT_PIN_DIFF(I2C_SCL, XSHUT_2);
ASSERT_PIN_DIFF(XSHUT_1, XSHUT_2);

ASSERT_PIN_DIFF(TRIG_PIN, ECHO_PIN);
ASSERT_PIN_DIFF(TRIG_PIN, SPI_SCK_PIN);
ASSERT_PIN_DIFF(TRIG_PIN, SS_PIN);
ASSERT_PIN_DIFF(ECHO_PIN, SPI_SCK_PIN);
ASSERT_PIN_DIFF(ECHO_PIN, SS_PIN);

ASSERT_PIN_DIFF(SPI_SCK_PIN, SPI_MISO_PIN);
ASSERT_PIN_DIFF(SPI_SCK_PIN, SPI_MOSI_PIN);
ASSERT_PIN_DIFF(SPI_SCK_PIN, SS_PIN);
ASSERT_PIN_DIFF(SPI_MISO_PIN, SPI_MOSI_PIN);
ASSERT_PIN_DIFF(SPI_MISO_PIN, SS_PIN);
ASSERT_PIN_DIFF(SPI_MOSI_PIN, SS_PIN);
ASSERT_PIN_DIFF(SS_PIN, IRQ_PIN);

ASSERT_PIN_DIFF(ENCODER_ESQ_A_PIN, ENCODER_ESQ_B_PIN);
ASSERT_PIN_DIFF(ENCODER_ESQ_A_PIN, ENCODER_DIR_A_PIN);
ASSERT_PIN_DIFF(ENCODER_ESQ_A_PIN, ENCODER_DIR_B_PIN);
ASSERT_PIN_DIFF(ENCODER_ESQ_B_PIN, ENCODER_DIR_A_PIN);
ASSERT_PIN_DIFF(ENCODER_ESQ_B_PIN, ENCODER_DIR_B_PIN);
ASSERT_PIN_DIFF(ENCODER_DIR_A_PIN, ENCODER_DIR_B_PIN);

#endif
