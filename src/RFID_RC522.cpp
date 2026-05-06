#include "../include/RFID_RC522.h"
#include "../ConfigPinos.h"

MFRC522 mfrc522(SS_PIN, RST_PIN); 

void initRFID() {
    SPI.begin();
    mfrc522.PCD_Init();
}

String lerTagRFID() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return "";
    }
    String conteudo = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    mfrc522.PICC_HaltA();
    conteudo.toUpperCase();
    return conteudo.substring(1);
}