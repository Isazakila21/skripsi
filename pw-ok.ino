#include <DHT.h>

// Pin setup
#define DHTPIN 2         // Pin untuk data DHT22
#define DHTTYPE DHT22    // Tipe sensor
#define RELAY_PIN 7      // Pin untuk relay

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay mati awal
}

void loop() {
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Gagal membaca data suhu!");
    return;
  }

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.println(" °C");

  if (suhu > 25.0) {
    digitalWrite(RELAY_PIN, LOW); // Peltier ON (relay aktif)
    Serial.println("Peltier: ON (pendinginan)");
  } else if (suhu < 20.0) {
    digitalWrite(RELAY_PIN, HIGH); // Peltier OFF (relay nonaktif)
    Serial.println("Peltier: OFF (suhu cukup dingin)");
  } else {
    Serial.println("Suhu stabil, Peltier tetap OFF");
  }

  delay(2000); // Tunggu 2 detik sebelum membaca lagi
}
