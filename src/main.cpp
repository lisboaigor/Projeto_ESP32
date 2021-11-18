#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "wifiConfig.h"

/* Definições do Telegram */
#define TIME_MSG_CHECK 250 //ms

/* Token de acesso Telegram */
#define telegram_token " " /* Coloque aqui o seu token de acesso Telegram (fornecido pelo BotFather) */

/* Definições das mensagens possíveis de serem recebidas */
#define CMD_PORTAO_ABRIR_1 "ABRIR"
#define CMD_PORTAO_FECHAR_1 "FECHAR"

WiFiClientSecure client;
UniversalTelegramBot bot(telegram_token, client);
unsigned long timestamp_msg_check = 0;
unsigned int msg_amount = 0;
bool status;

bool WiFiConnect()
{
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);

        if ((++i % 16) == 0)
            Serial.println("Tentando conectar...");
    }
    Serial.print(F("Conexão estabelecida!"));
    Serial.println(WiFi.localIP());
    return true;
}

void init_wifi()
{
    Serial.println("-------CONTROLE_PORTAO-------");
    Serial.print("Conectando-se a rede:");
    Serial.println(SSID);
    Serial.println("Aguarde...");
    WiFiConnect();
}

bool isConnected()
{
    return WiFiConnect();
}

unsigned long time_difference(unsigned long timestamp)
{
    return (millis() - timestamp);
}

void openGate()
{
}

void closeGate()
{
}

String decode_message(String msg)
{
    bool isValidCommand = false;

    if (msg.equals(CMD_PORTAO_ABRIR_1) && status == false)
        openGate();
    else if (msg.equals(CMD_PORTAO_FECHAR_1) && status == true)
        closeGate();
}

void setup()
{
    Serial.begin(9600);
    init_wifi();
    timestamp_msg_check = millis();
}

void loop()
{
   
}