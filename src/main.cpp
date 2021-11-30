#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include "wifiConfig.h"

// Definições do Telegram
#define telegram_token "2103501476:AAGVqUHNVZ8r1xIIp2-uc07p0PDlMHDalg0"
#define TIME_MSG_CHECK 1000 // Checa se há mensagens a cada 1 segundo

// Definições das mensagens possíveis de serem recebidas
#define OPEN_GATE_CMD "ABRIR"
#define CLOSE_GATE_CMD "FECHAR"

// Status do portão
bool status;

// Variáveis do usadas pelo bot
String msg, id;
unsigned long bot_last_time; // Horario da ultima mensagem enviada
unsigned long msg_amount;    // Quantidade de mensagens recebidas

// Configuração do bot com o cliente WIFI
WiFiClientSecure client;
UniversalTelegramBot bot(telegram_token, client);

void WiFiConnect()
{

    while (WiFi.status() != WL_DISCONNECTED)
    {
        delay(1000);
        Serial.println("Tentando estabelecer conexão...");
        WiFi.begin(SSID, WiFiPassword);
    }

    Serial.println(F("Conexão estabelecida!"));
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
}

void wifi_init()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, WiFiPassword);

    #ifdef ESP32
        client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
    #endif

    Serial.println("-------CONTROLE PORTÃO-------");
    Serial.print("Conectando-se à rede:");
    Serial.println(SSID);

    WiFiConnect();
}

void openGate(String id)
{
    for (unsigned i = 0; i < 6; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
    }

    status = true;
    bot.sendMessage(id, "Portão aberto!");
}

void closeGate(String id)
{
    for (unsigned i = 0; i < 12; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
    }

    status = false;
    bot.sendMessage(id, "Portão fechado!");
}

void read_messages()
{
    msg_amount = bot.getUpdates(bot.last_message_received + 1);

    for (unsigned long i = 0; i < msg_amount; i++)
    {
        id = bot.messages[i].chat_id;
        msg = bot.messages[i].text;
        msg.toUpperCase();

        if (msg.equals(OPEN_GATE_CMD) && status == false)
            openGate(id);
        else if (msg.equals(OPEN_GATE_CMD) && status == true)
            bot.sendMessage(id, "O portão já está aberto!");
        else if (msg.equals(CLOSE_GATE_CMD) && status == true)
            closeGate(id);
        else if (msg.equals(CLOSE_GATE_CMD) && status == false)
            bot.sendMessage(id, "O portão já está fechado!");
        else
            bot.sendMessage(id, "Comando inválido!");
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    msg_amount = 0;
    wifi_init();
}

void loop()
{
    if (millis() - bot_last_time > TIME_MSG_CHECK)
    {
        msg_amount = bot.getUpdates(bot.last_message_received + 1);

        while (msg_amount)
        {
            read_messages();
            msg_amount = bot.getUpdates(bot.last_message_received + 1);
        }

        bot_last_time = millis();
    }
}