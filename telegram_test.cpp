#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

TFT_eSPI tft = TFT_eSPI();

// POT
#define POT 27

// HC-SR04
#define TRIG 26
#define ECHO 14

// Buzzer
#define BUZZER 32

// LED
#define LED 2

// WiFi
const char* ssid = "nazwa";
const char* password = "haslo";

// Telegram
#define BOT_TOKEN "bot_token"
#define CHAT_ID "chat_id"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long ostatniTelegram = 0;

int licznik = 0;
bool wykryto = false;

void sendTelegram(int odleglosc, int licznik)
{
  String msg =
      "Wykryto osobe!\n\n"
      "Odleglosc: " + String(odleglosc) + " cm\n" +
      "Licznik: " + String(licznik);

  if (bot.sendMessage(CHAT_ID, msg, ""))
  {
    Serial.println("Telegram wyslany");
  }
  else
  {
    Serial.println("Blad wysylania Telegram");
  }
}

void setup()
{
  Serial.begin(115200);

  // WiFi
  WiFi.begin(ssid, password);

  Serial.print("Laczenie z WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi polaczone");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();

  if (bot.sendMessage(CHAT_ID,
                      "ESP32 uruchomione i gotowe do pracy.",
                      ""))
  {
    Serial.println("Telegram OK");
  }
  else
  {
    Serial.println("Telegram FAIL");
  }

  // Piny
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
  tft.print("LICZNIK OSOB");

  delay(1000);
}

void loop()
{
  // Potencjometr
  int potValue = analogRead(POT);

  // Prog od 20 do 200 cm
  int prog = map(potValue, 0, 4095, 20, 200);

  // Pomiar HC-SR04
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long czas = pulseIn(ECHO, HIGH, 30000);

  int odleglosc = czas * 0.034 / 2;

  Serial.print("Odleglosc: ");
  Serial.print(odleglosc);
  Serial.print(" cm   Prog: ");
  Serial.println(prog);

  // LED
  if (odleglosc < prog)
  {
    digitalWrite(LED, HIGH);
  }
  else
  {
    digitalWrite(LED, LOW);
  }

  // Wykrycie osoby
  if (odleglosc < prog && !wykryto)
  {
    licznik++;
    wykryto = true;

    // Telegram maksymalnie raz na minute
    if (millis() - ostatniTelegram > 60000)
    {
      sendTelegram(odleglosc, licznik);
      ostatniTelegram = millis();
    }

    tone(BUZZER, 1500);
    delay(200);
    noTone(BUZZER);
  }

  if (odleglosc > prog && wykryto)
  {
    wykryto = false;
  }

  // TFT
  tft.fillScreen(TFT_WHITE);

  tft.setTextColor(TFT_BLACK);

  tft.setCursor(20, 20);
  tft.print("ODLEGLOSC");

  tft.setCursor(20, 50);
  tft.print(odleglosc);
  tft.print(" cm");

  tft.setCursor(20, 110);
  tft.print("LICZNIK");

  tft.setCursor(20, 140);
  tft.print(licznik);

  tft.setCursor(20, 220);
  tft.print("PROG: ");
  tft.print(prog);
  tft.print(" cm");

  delay(100);
}
