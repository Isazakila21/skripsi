#include <Wire.h>
#include "model_akurat.h"   // model Decision Tree hasil export dari Python
#include <Adafruit_TCS34725.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// === Konfigurasi Pin ===
#define DHTPIN 2
#define DHTTYPE DHT22
#define MQ3_PIN A0
#define RELAY_PIN 7
#define SWITCH_PIN 3

// === Konstanta Kalibrasi MQ-3 ===
const float Vc = 5.0;        // Tegangan kerja sensor
const float RL = 10000.0;    // Resistor load 10k
const float R0 = 512.64;     // R0 hasil kalibrasi di dalam chamber

// Persamaan linearitas: persen = m * tegangan + c
const float m = 0.0496;
const float c = 0.0374;

// Inisialisasi sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);
Eloquent::ML::Port::SVM model;


const char* label[] = {"Mentah", "Matang", "Terlalu Matang"};

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);  // Gunakan resistor internal pull-up

  dht.begin();

  if (!tcs.begin()) {
    Serial.println("Sensor warna tidak terdeteksi");
    while (1);
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Siap...");
  Serial.println("Sistem Siap...");
  delay(2000);
  lcd.clear();
}

void loop() {
  bool switchState = digitalRead(SWITCH_PIN) == LOW; // LOW = tombol ditekan

  if (!switchState) {
    // ==================== MODE SUHU + PENDINGIN ====================
    float suhu = dht.readTemperature();
    float kelembapan = dht.readHumidity();

    if (isnan(suhu) || isnan(kelembapan)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Error DHT22");
      delay(2000);
      return;
    }

    // Tampilkan suhu dan kelembapan
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("S:");
    lcd.print(suhu, 1);
    lcd.print((char)223);
    lcd.print("C  H:");
    lcd.print(kelembapan, 0);
    lcd.print("%");

    // Kontrol Peltier berdasarkan suhu
    lcd.setCursor(0, 1);
    if (suhu > 25.0) {
      digitalWrite(RELAY_PIN, LOW); // Peltier ON
      lcd.print("Pendingin ON ");
      Serial.println("Peltier: ON");
    } else if (suhu < 20.0) {
      digitalWrite(RELAY_PIN, HIGH); // Peltier OFF
      lcd.print("Pendingin OFF");
      Serial.println("Peltier: OFF");
    } else {
      lcd.print("Suhu Stabil:OK");
      Serial.println("Peltier: OFF (Stabil)");
    }

    // Kirim ke Serial Monitor
    Serial.print("Suhu: "); Serial.print(suhu); Serial.print(" C | ");
    Serial.print("Kelembapan: "); Serial.print(kelembapan); Serial.println(" %");
    delay(3000);

  } else {
    // ==================== MODE DETEKSI PISANG ====================
    // 1. Baca sensor MQ-3 (alkohol)
    int mq3Value = analogRead(MQ3_PIN);
    float tegangan_mq3 = mq3Value * (5.0 / 1023.0);

    // Hitung Rs
    float sensorRs = RL * (Vc - tegangan_mq3) / tegangan_mq3;

    // Hitung rasio Rs/R0
    float ratio = sensorRs / R0;

    // Hitung persen alkohol dari persamaan kalibrasi
    float persen_alkohol = (m * tegangan_mq3) + c;
    if (persen_alkohol < 0) persen_alkohol = 0; // hindari negatif

    // === Tampilan LCD format baru ===
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alkohol");

    lcd.setCursor(0, 1);
    lcd.print(tegangan_mq3, 2);
    lcd.print(" V (");
    lcd.print(persen_alkohol, 2);
    lcd.print("%)");

    // Serial monitor
    Serial.print("Tegangan: "); Serial.print(tegangan_mq3, 3); Serial.print(" V | ");
    Serial.print("Rs/R0: "); Serial.print(ratio, 2); Serial.print(" | ");
    Serial.print("Alkohol: "); Serial.print(persen_alkohol, 2); Serial.println(" %");
    delay(5000);

    // 2. Baca sensor warna TCS34725
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);

    // Nilai mean dan scale hasil dari StandardScaler
    float mean_vals[3] = {78.7, 72.86388889, 29.31666667};
    float scale_vals[3] = {38.36041073, 34.56123111, 14.57679854};

    // Lakukan normalisasi RGB sebelum prediksi
    float input[3];
    input[0] = (r - mean_vals[0]) / scale_vals[0];
    input[1] = (g - mean_vals[1]) / scale_vals[1];
    input[2] = (b - mean_vals[2]) / scale_vals[2];

    int hasil = model.predict(input);


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("R:"); lcd.print(r);
    lcd.print(" G:"); lcd.print(g);
    lcd.setCursor(0, 1);
    lcd.print("B:"); lcd.print(b);

    Serial.print("RGB: ");
    Serial.print(r); Serial.print(", ");
    Serial.print(g); Serial.print(", ");
    Serial.println(b);
    delay(5000);

    // 3. Klasifikasi kondisi pisang menggunakan model
    //float input[3] = {(float)r, (float)g, (float)b};
    //int hasil = model.predict(input);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kondisi:");
    lcd.setCursor(0, 1);

    if (hasil >= 0 && hasil < 3) {
      lcd.print(label[hasil]);
      Serial.print("Kondisi Pisang: ");
      Serial.println(label[hasil]);
    } else {
      lcd.print("Tidak diketahui");
      Serial.println("Kondisi: Tidak diketahui");
    }

    delay(5000);
  }
}
