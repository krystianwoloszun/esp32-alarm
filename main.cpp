#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

TFT_eSPI tft = TFT_eSPI();
//POT
#define POT 27

// HC-SR04
#define TRIG 26
#define ECHO 14

// Buzzer
#define BUZZER 32

// LED (wbudowana LED ESP32)
#define LED 2

SMTPSession smtp;

const char* ssid = "wifi_example_name";
const char* password = "12345";

#define AUTHOR_EMAIL "mail@example.com"
#define AUTHOR_PASSWORD "example"

#define RECIPIENT_EMAIL "recipient@example.com"

unsigned long ostatniMail = 0;


int licznik = 0;
bool wykryto = false;

const int prog = 100; // [cm]

void setup() {

  Serial.begin(115200);
  
WiFi.begin(ssid, password);

Serial.print("Laczenie z WiFi");

while (WiFi.status() != WL_CONNECTED)
{
  delay(500);
  Serial.print(".");
}

Serial.println();
Serial.println("WiFi polaczone");
Serial.println(WiFi.localIP());


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

void sendEmail(int odleglosc, int licznik)
{
  SMTP_Message message;

  message.sender.name = "ESP32 Licznik";
  message.sender.email = AUTHOR_EMAIL;

  message.subject = "Wykryto osobe";

  message.addRecipient("Karolina", RECIPIENT_EMAIL);

  String tekst =
    "Wykryto osobe.\n\n"
    "Odleglosc: " + String(odleglosc) + " cm\n" +
    "Licznik: " + String(licznik);

  message.text.content = tekst.c_str();

  Session_Config config;

  config.server.host_name = "smtp.gmail.com";
  config.server.port = 465;

  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;

  smtp.connect(&config);

  if (MailClient.sendMail(&smtp, &message))
  {
    Serial.println("Email wyslany");
  }
  else
  {
    Serial.println("Blad wysylki");
  }
}
void loop() {


  // Potencjometr
  int potValue = analogRead(POT);

  // 20-200 [cm]
  int prog = map(potValue, 0, 4095, 20, 200);

  // Pomiar odległości HC-SR04
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long czas = pulseIn(ECHO, HIGH);

  int odleglosc = czas * 0.034 / 2;

  Serial.print("Odleglosc: ");
  Serial.print(odleglosc);

  Serial.print(" cm  Prog: ");
  Serial.println(prog);

  // LED
  if (odleglosc < prog) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }

if (odleglosc < prog && !wykryto) {

  licznik++;
  wykryto = true;

  if (millis() - ostatniMail > 60) {
    sendEmail(odleglosc, licznik);
    ostatniMail = millis();
  }

  tone(BUZZER, 1500);
  delay(200);
  noTone(BUZZER);
}
  if (odleglosc > prog && wykryto) {
    wykryto = false;
  }

  tft.fillScreen(TFT_WHITE);

  tft.setTextColor(TFT_BLACK);
  tft.setCursor(20, 20);
  tft.print("ODLEGLOSC");

  tft.setTextColor(TFT_BLACK);
  tft.setCursor(20, 50);
  tft.print(odleglosc);
  tft.print(" cm");

  tft.setTextColor(TFT_BLACK);
  tft.setCursor(20, 110);
  tft.print("LICZNIK");

  tft.setTextColor(TFT_BLACK);
  tft.setCursor(20, 140);
  tft.print(licznik);

  tft.setTextColor(TFT_BLACK);
  tft.setCursor(20, 220);
  tft.print("PROG: ");
  tft.print(prog);
  tft.print(" cm");


  delay(100);
}

