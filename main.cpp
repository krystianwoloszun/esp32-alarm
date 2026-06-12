#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

TFT_eSPI tft = TFT_eSPI();

#define POT 27
#define TRIG 26
#define ECHO 14
#define BUZZER 32
#define LED 2

// WiFi
const char* ssid = "example_ssid";
const char* password = "ultra_safe_password";

// Telegram
#define BOT_TOKEN "example_bot_token"
#define CHAT_ID "example_chat_id"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastTelegram = 0;

int counter = 0;
bool detected = false;

void sendTelegram(int distance, int counter)
{
  String msg =
      "🚨 Movement detect!\n\n"
      "Distance: " + String(distance) + " cm\n" +
      "Count: " + String(counter);

  if (bot.sendMessage(CHAT_ID, msg, ""))
  {
    Serial.println("Telegram sent");
  }
  else
  {
    Serial.println("Error: Could not send a Telegram");
  }
}

void setup()
{
  Serial.begin(115200);

  // WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();

  if (bot.sendMessage(CHAT_ID,
                      "ESP32 ready.",
                      ""))
  {
    Serial.println("Telegram OK");
  }
  else
  {
    Serial.println("Telegram FAIL");
  }

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, LOW);

  // TFT
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_WHITE);

  tft.setTextSize(2);

  tft.setTextColor(TFT_GREEN);
  tft.setCursor(30, 10);
  tft.print("COUTER");

  delay(1000);
}

void loop()
{
  int potValue = analogRead(POT);

  // 20 - 200 [cm]
  int threshold = map(potValue, 0, 4095, 20, 200);

  // HC-SR04 measurement
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long time = pulseIn(ECHO, HIGH, 30000);

  int distance = time * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm   Threshold: ");
  Serial.println(threshold);

  // LED
  if (distance < threshold)
  {
    digitalWrite(LED, HIGH);
  }
  else
  {
    digitalWrite(LED, LOW);
  }

  // Movement detection
  if (distance < threshold && !detected)
  {
    counter++;
    detected = true;

    // Once per minute
    if (millis() - lastTelegram > 60000)
    {
      sendTelegram(distance, counter);
      lastTelegram = millis();
    }

    tone(BUZZER, 1500);
    delay(200);
    noTone(BUZZER);
  }

  if (distance > threshold && detected)
  {
    detected = false;
  }

  // TFT
  tft.fillScreen(TFT_WHITE);

  tft.setTextColor(TFT_BLACK);

  tft.setCursor(20, 20);
  tft.print("DISTANCE");

  tft.setCursor(20, 50);
  tft.print(distance);
  tft.print(" cm");

  tft.setCursor(20, 110);
  tft.print("COUNTER");

  tft.setCursor(20, 140);
  tft.print(counter);

  tft.setCursor(20, 220);
  tft.print("THRESHOLD: ");
  tft.print(threshold);
  tft.print(" cm");

  delay(100);
}
