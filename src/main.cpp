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
#define OPEN_GATE_CMD "/abrir"
#define CLOSE_GATE_CMD "/fechar"

// Status do portão
bool status;

// Variáveis do usadas pelo bot
String msg, id;
uint32_t bot_last_time; // Horario da ultima mensagem enviada

// Configuração do bot com o cliente WIFI
WiFiClientSecure client;
UniversalTelegramBot bot(telegram_token, client);

void setupWiFi()
{
    WiFi.begin(ssid, password);
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

    Serial.println("-------CONTROLE PORTÃO-------");
    Serial.print("Conectando-se à rede:");
    Serial.println(ssid);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Tentando estabelecer conexão...");
    }

    Serial.print(F("Conexão estabelecida! Endereço IP: "));
    Serial.println(WiFi.localIP());
}

void openGate(String id)
{
    bot.sendMessage(id, "Abrindo o portão...");
    
    for (uint8_t i = 0; i < 6; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        Serial.println("LED ON");
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        Serial.println("LED OFF");
    }

    status = true;
    bot.sendMessage(id, "Portão aberto!");
}

void closeGate(String id)
{
    bot.sendMessage(id, "Fechando o portão...");
    for (uint8_t i = 0; i < 12; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        Serial.println("LED ON");
        digitalWrite(LED_BUILTIN, LOW);
        delay(250);
        Serial.println("LED OFF");
    }

    status = false;
    bot.sendMessage(id, "Portão fechado!");
}

void read_messages(uint32_t *msg_amount)
{
    for (uint32_t i = 0; i < *msg_amount; i++)
    {
        id = bot.messages[i].chat_id;
        msg = bot.messages[i].text;

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
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    setupWiFi();
}

void loop()
{
    if (millis() - bot_last_time > TIME_MSG_CHECK)
    {
        uint32_t msg_amount = bot.getUpdates(bot.last_message_received + 1);

        while (msg_amount)
        {
            read_messages(&msg_amount);
            msg_amount = bot.getUpdates(bot.last_message_received + 1);
        }

        bot_last_time = millis();
    }
}
