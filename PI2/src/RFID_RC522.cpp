#include "../include/RFID_RC522.h"
#include "../ConfigPinos.h"

MFRC522 mfrc522(SS_PIN, RST_PIN); 

const byte BLOCO_DADOS_RFID = 1;
const byte TAMANHO_BLOCO_RFID = 16;

void imprimirDadosGravadosRFID() {
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
        MFRC522::PICC_CMD_MF_AUTH_KEY_A,
        BLOCO_DADOS_RFID,
        &key,
        &(mfrc522.uid)
    );
    if (status != MFRC522::STATUS_OK) {
        Serial.print("[RFID] ERRO autenticando bloco ");
        Serial.print(BLOCO_DADOS_RFID);
        Serial.print(": ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    byte buffer[18] = {0};
    byte tamanho = sizeof(buffer);
    status = mfrc522.MIFARE_Read(BLOCO_DADOS_RFID, buffer, &tamanho);
    if (status != MFRC522::STATUS_OK) {
        Serial.print("[RFID] ERRO lendo bloco ");
        Serial.print(BLOCO_DADOS_RFID);
        Serial.print(": ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    byte tamanhoTexto = TAMANHO_BLOCO_RFID;
    while (tamanhoTexto > 0 && (buffer[tamanhoTexto - 1] == 0x00 || buffer[tamanhoTexto - 1] == ' ')) {
        tamanhoTexto--;
    }

    Serial.print("[RFID] Dados ASCII: ");
    if (tamanhoTexto == 0) {
        Serial.println("(vazio)");
    } else {
        for (byte i = 0; i < tamanhoTexto; i++) {
            if (buffer[i] >= 32 && buffer[i] <= 126) {
                Serial.write(buffer[i]);
            } else {
                Serial.print(".");
            }
        }
        Serial.println();
    }

    Serial.print("[RFID] Dados HEX: ");
    for (byte i = 0; i < TAMANHO_BLOCO_RFID; i++) {
        if (buffer[i] < 0x10) {
            Serial.print("0");
        }
        Serial.print(buffer[i], HEX);
        if (i + 1 < TAMANHO_BLOCO_RFID) {
            Serial.print(" ");
        }
    }
    Serial.println();
    mfrc522.PCD_StopCrypto1();
}

#if RFID_USAR_IRQ
volatile bool rfidInterrompeu = false;
volatile bool interrupcaoRFIDAtiva = true;
bool rfidBuscaArmada = false;

void IRAM_ATTR tratarInterrupcaoRFID() {
    if (interrupcaoRFIDAtiva) {
        rfidInterrompeu = true;
        interrupcaoRFIDAtiva = false;
        detachInterrupt(digitalPinToInterrupt(IRQ_PIN));
    }
}

void armarBuscaRFIDPorIRQ() {
    detachInterrupt(digitalPinToInterrupt(IRQ_PIN));
    noInterrupts();
    rfidInterrompeu = false;
    interrupcaoRFIDAtiva = false;
    interrupts();

    mfrc522.PCD_WriteRegister(MFRC522::CommandReg, MFRC522::PCD_Idle);
    mfrc522.PCD_WriteRegister(MFRC522::ComIrqReg, 0x7F);
    mfrc522.PCD_WriteRegister(MFRC522::ComIEnReg, 0xA0);
    mfrc522.PCD_WriteRegister(MFRC522::FIFOLevelReg, 0x80);
    mfrc522.PCD_WriteRegister(MFRC522::FIFODataReg, MFRC522::PICC_CMD_REQA);
    mfrc522.PCD_WriteRegister(MFRC522::BitFramingReg, 0x07);
    mfrc522.PCD_WriteRegister(MFRC522::CommandReg, MFRC522::PCD_Transceive);
    mfrc522.PCD_SetRegisterBitMask(MFRC522::BitFramingReg, 0x80);
    rfidBuscaArmada = true;

    noInterrupts();
    interrupcaoRFIDAtiva = true;
    interrupts();
    attachInterrupt(digitalPinToInterrupt(IRQ_PIN), tratarInterrupcaoRFID, CHANGE);
}

void recuperarBuscaRFIDPorIRQ(bool reiniciarLeitor = false) {
    detachInterrupt(digitalPinToInterrupt(IRQ_PIN));
    noInterrupts();
    rfidInterrompeu = false;
    interrupcaoRFIDAtiva = false;
    interrupts();

    mfrc522.PCD_WriteRegister(MFRC522::CommandReg, MFRC522::PCD_Idle);
    mfrc522.PCD_WriteRegister(MFRC522::ComIrqReg, 0x7F);
    if (reiniciarLeitor) {
        mfrc522.PCD_Init();
    }
    armarBuscaRFIDPorIRQ();
}
#endif

void initRFID() {
    Serial.println("[RFID] Inicializando barramento SPI...");
    Serial.printf("[RFID] SPI SCK=GPIO %d | MISO=GPIO %d | MOSI=GPIO %d | SS/CS=GPIO %d | RST=GPIO %d | IRQ=GPIO %d\n",
                  SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SS_PIN, RST_PIN, IRQ_PIN);

    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SS_PIN);
    mfrc522.PCD_Init();

    byte version = mfrc522.PCD_ReadRegister(MFRC522::VersionReg);
    Serial.print("[RFID] VersionReg: 0x");
    Serial.println(version, HEX);
    if (version == 0x00 || version == 0xFF) {
        Serial.println("[RFID] ERRO: RC522 nao respondeu. Verifique alimentacao 3.3V, GND, SCK, MISO, MOSI, SS e RST.");
    } else {
        Serial.println("[RFID] RC522 respondeu no SPI.");
    }

#if RFID_USAR_IRQ
    pinMode(IRQ_PIN, INPUT_PULLUP);
    armarBuscaRFIDPorIRQ();
    Serial.print("[RFID] Estado inicial IRQ: ");
    Serial.println(digitalRead(IRQ_PIN));
    Serial.println("[RFID] Interrupcao habilitada somente para RxIRq. Polling de leitura desativado.");
#else
    Serial.println("[RFID] IRQ desabilitada. Usando polling rapido a cada 50 ms.");
#endif
    Serial.println("[RFID] Aproxime uma tag/cartao do leitor.");
}

static String processarTagDetectada() {
    static unsigned long ultimaLeitura = 0;
    static String ultimoUID = "";

    Serial.println("[RFID] Tag detectada. Lendo UID...");
    if (!mfrc522.PICC_ReadCardSerial()) {
        Serial.println("[RFID] ERRO: falha ao ler serial da tag.");
        return "";
    }

    String conteudo = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    conteudo.toUpperCase();
    conteudo = conteudo.substring(1);

    if (conteudo == ultimoUID && millis() - ultimaLeitura < 1500) {
        return "";
    }
    ultimoUID = conteudo;
    ultimaLeitura = millis();

    Serial.print("[RFID] UID: ");
    Serial.println(conteudo);
    imprimirDadosGravadosRFID();
    mfrc522.PICC_HaltA();
    
    return conteudo;
}

#if RFID_USAR_IRQ

String lerTagRFID() {
    static unsigned long ultimoLogVida = 0;
    static unsigned long ultimaRearmada = 0;

    if (!rfidBuscaArmada || millis() - ultimaRearmada > 1000) {
        armarBuscaRFIDPorIRQ();
        ultimaRearmada = millis();
    }

    if (rfidInterrompeu) {
        noInterrupts();
        rfidInterrompeu = false;
        interrupts();
        rfidBuscaArmada = false;

        byte irqFlags = mfrc522.PCD_ReadRegister(MFRC522::ComIrqReg);
        byte errorFlags = mfrc522.PCD_ReadRegister(MFRC522::ErrorReg);
        Serial.print("[RFID] IRQ recebida. ComIrqReg=0x");
        Serial.print(irqFlags, HEX);
        Serial.print(" ErrorReg=0x");
        Serial.println(errorFlags, HEX);

        if ((irqFlags & 0x20) == 0) {
            recuperarBuscaRFIDPorIRQ(false);
            return "";
        }

        mfrc522.PCD_WriteRegister(MFRC522::CommandReg, MFRC522::PCD_Idle);
        mfrc522.PCD_WriteRegister(MFRC522::ComIrqReg, 0x7F);
        mfrc522.PCD_WriteRegister(MFRC522::BitFramingReg, 0x00);
        Serial.println("[RFID] RxIRq confirmado. Lendo UID...");

        String tag = processarTagDetectada();
        
        if (tag == "") {
            recuperarBuscaRFIDPorIRQ(true);
        } else {
            recuperarBuscaRFIDPorIRQ(false);
        }
        return tag;
    } else {
        if (millis() - ultimoLogVida > 2000) {
            Serial.print("[RFID] Aguardando IRQ. Estado IRQ=");
            Serial.println(digitalRead(IRQ_PIN));
            ultimoLogVida = millis();
        }
        return "";
    }
}

#else

String lerTagRFID() {
    static unsigned long ultimaVarredura = 0;
    static unsigned long ultimoLogVida = 0;
    static unsigned long ultimoLogBusca = 0;

    if (millis() - ultimoLogVida > 2000) {
        Serial.println("[RFID] Loop ativo.");
        ultimoLogVida = millis();
    }

    if (millis() - ultimaVarredura < 50) {
        return "";
    }
    ultimaVarredura = millis();

    if (!mfrc522.PICC_IsNewCardPresent()) {
        if (millis() - ultimoLogBusca > 1000) {
            Serial.println("[RFID] Procurando tag por polling...");
            ultimoLogBusca = millis();
        }
        return "";
    }

    return processarTagDetectada();
}

#endif
