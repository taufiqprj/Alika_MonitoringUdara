#define BLYNK_TEMPLATE_ID "TMPL6YMSszWTi"
#define BLYNK_TEMPLATE_NAME "Monitoring Kualitas Udara"
#define BLYNK_AUTH_TOKEN "IX6gNTGtSFEakPZRcAICl8gA5eBw-GwJ"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <MQ135.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

char auth[] = "IX6gNTGtSFEakPZRcAICl8gA5eBw-GwJ";
char ssid[] = "Nyangz";
char pass[] = "nazhtlma";

#define PIN_MQ135 A0
#define DHTPIN D4
#define DHTTYPE DHT11
#define PIN_BUZZER D5

MQ135 mq135_sensor(PIN_MQ135);
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Alamat I2C mungkin perlu disesuaikan

float temperature = 21.0;
float humidity = 25.0;
float rzero, correctedRZero, resistance, ppm, correctedPPM;
float humi, temp;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Monitoring"); 
  lcd.setCursor(0, 1);
  lcd.print("Kualitas Udara");

  Blynk.begin(auth, ssid, pass);
  dht.begin();
  delay(2000);
  lcd.clear();
  pinMode(PIN_BUZZER, OUTPUT);
}

void readDHT11() {
  humi = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(humi) || isnan(temp)) {
    Serial.println("DHT11 tidak terbaca... !");
    return;
  }
  Serial.print("Suhu=");
  Serial.print(temp);
  Serial.println(" Celsius");
  Serial.print("Humi=");
  Serial.print(humi);
  Serial.println(" %RH");
  Serial.println();
}

void readMQ135() {
  rzero = mq135_sensor.getRZero();
  correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  resistance = mq135_sensor.getResistance();
  ppm = mq135_sensor.getPPM();
  correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM, 1);
  Serial.println("ppm");
}

void loop() {
  readMQ135();
  readDHT11();

  Blynk.virtualWrite(V0, correctedPPM);
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, humi);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AQ:");
  lcd.print(correctedPPM, 1);
  lcd.print("PPM");
  lcd.print(" T:");
  lcd.print(temp, 1);
  lcd.print("C ");

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(humi, 1);
  lcd.print("%");

  if (ppm <= 50) {
    Blynk.virtualWrite(V3, "Fresh Air");
    lcd.setCursor(8, 1);
    lcd.print("St:");
    lcd.print("Fresh");
    digitalWrite(PIN_BUZZER, HIGH);
  } else if (ppm >= 50 && ppm <= 100) {
    Blynk.virtualWrite(V3, "Moderate");
    lcd.setCursor(8, 1);
    lcd.print("St:");
    lcd.print("Poor");
    digitalWrite(PIN_BUZZER, LOW);
  } else if (ppm >= 100 && ppm <= 200) {
    Blynk.virtualWrite(V3, "Unhealty");
    lcd.setCursor(8, 1);
    lcd.print("St:");
    lcd.print("Danger");
    digitalWrite(PIN_BUZZER, LOW);
  } else if (ppm >= 200 && ppm <= 300) {
    Blynk.virtualWrite(V3, "Very unhealthy");
    lcd.setCursor(8, 1);
    lcd.print("St:");
    lcd.print("Danger");
    digitalWrite(PIN_BUZZER, LOW);
  } else if (ppm >= 300 && ppm <= 500) {
    Blynk.virtualWrite(V3, "Hazardous");
    lcd.setCursor(8, 1);
    lcd.print("St:");
    lcd.print("Danger");
    digitalWrite(PIN_BUZZER, LOW);
  }

  delay(2000);

}