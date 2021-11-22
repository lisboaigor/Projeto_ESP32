#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include "wifiConfig.h"

/* Definições do Telegram */
#define TIME_MSG_CHECK 250 //ms

/* Token de acesso Telegram */
#define telegram_token ""

/* Definições das mensagens possíveis de serem recebidas */
#define OPEN_GATE_CMD  "ABRIR"
#define CLOSE_GATE_CMD "FECHAR"
#define LOCK_GATE_CMD  "TRAVAR ABERTA"

// Status do portão
bool status;
bool locked_status;

// Resposta do Bot
String bot_answer;
unsigned long last_message_index;

WiFiClientSecure client;
UniversalTelegramBot bot(telegram_token, client);

bool WiFiConnect()
{
    uint8_t i = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);

        if ((++i % 16) == 0)
            Serial.println("Tentando conectar...");
    }

    Serial.print(F("Conexão estabelecida!"));
    Serial.println(WiFi.localIP());

    return true;
}

void wifi_init()
{
    Serial.println("-------CONTROLE_PORTAO-------");
    Serial.print("Conectando-se a rede:");
    Serial.println(SSID);
    Serial.println("Aguarde...");

    WiFiConnect();
}

void openGate()
{
    for (unsigned i = 0; i < 10; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }

    status = true;   
}

void closeGate()
{
    for (unsigned i = 0; i < 10; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
    }

    status = false;
}

void get_signal() {
    if (WiFi.status() == WL_CONNECTED)
        bot.getUpdates(bot.last_message_received + 1);
    else
        WiFiConnect();
}

void decode_message(String msg)
{
    if (msg.equals(OPEN_GATE_CMD) && status == false) 
    {
        openGate();
        
        for (unsigned i = 0; i < 15; i++)
        {
            get_signal();
            decode_message(bot.messages[bot.getUpdates(bot.last_message_received)].text);
            delay(1000);
        }

        if (status == true && locked_status == false)
            closeGate();
    }
    else if (msg.equals(CLOSE_GATE_CMD) && status == true) 
        closeGate();

    else if (msg.equals(LOCK_GATE_CMD) && status == true && locked_status == false)
        locked_status = true;
}

void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    
    wifi_init();
}

void loop()
{
    get_signal();
    decode_message(bot.messages[bot.getUpdates(bot.last_message_received)].text);
}