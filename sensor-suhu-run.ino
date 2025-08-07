#include <DHT.h>

#define DHTPIN 2         // Gunakan pin D2
#define DHTTYPE DHT22    // Tipe sensor

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000); // tunggu 2 detik antar pembacaan

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca sensor DHT22!");
    return;
  }

  Serial.print("Kelembapan: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.println(" °C");
}
