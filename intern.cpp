#include <iostream>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

using namespace std; 
// Replace with your network credentials
char customSSID[32] = "Half-life";
char customPassword[32] = "praj2004";

// Initialize Telegram BOT
#define BOTtoken "6775148469:AAEMJDcCnueE5ykaxFAca0qJZoMQyEnTD_k"  // your Bot Token (Get from Botfather)

String CHAT_ID[] = {"1244111832", "6602706709","1341812411"};
int array_size=sizeof(CHAT_ID);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 500;
unsigned long lastTimeBotRan;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  int flag = 0;
  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    for (int j = 0; j <= array_size; j++) {
      if (chat_id == CHAT_ID[j]) {
        flag = 1;
      }
    }

    if (flag == 0) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/on to turn GPIO ON \n";
      welcome += "/off to turn GPIO OFF \n";
      welcome += "/state to request current GPIO state \n";

      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/on") {
      if (digitalRead(4)) {
        bot.sendMessage(chat_id, "LED is already ON", "");
      } else {
        digitalWrite(4, HIGH);
        bot.sendMessage(chat_id, "LED state set to ON", "");
      }
    }

    if (text == "/off") {
      digitalWrite(4, LOW);  // Turn off the LED
      bot.sendMessage(chat_id, "LED turned off", "");  // Send success message
    }

    if (text == "/state") {
      if (digitalRead(4)) {
        bot.sendMessage(chat_id, "LED is ON", "");
      } else {
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(4, OUTPUT);
  //digitalWrite(4, LOW);

  // Create an instance of WiFiManager
  WiFiManager wifiManager;

  // Uncomment the following line for testing purposes, to reset WiFi credentials
  //wifiManager.resetSettings();

  // Try to connect to WiFi with saved credentials or create a captive portal
  if (!wifiManager.autoConnect("ESP32-Setup")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    // Reset and try again, or put your code to handle the failure here
    ESP.restart();
    delay(5000);
  }

  // Get and print the updated WiFi credentials
  strcpy(customSSID, WiFi.SSID().c_str());
  strcpy(customPassword, WiFi.psk().c_str());
  Serial.println("WiFi connected with updated credentials");
  Serial.print("SSID: ");
  Serial.println(customSSID);
  Serial.print("Password: ");
  Serial.println(customPassword);

  // Initialize the Telegram Bot
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}