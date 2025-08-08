const int mq3Pin = A0;
const float Vc = 5.0;   // Tegangan referensi
const float m = 0.0435; // Koefisien slope dari kalibrasi
const float c = 0.23; // Intercept dari kalibrasi

void setup() {
  Serial.begin(9600);
  Serial.println("=== MQ-3 Pengukuran Alkohol (%) ===");
}

void loop() {
  int adcValue = analogRead(mq3Pin);
  float voltage = (adcValue / 1023.0) * Vc;

  // Hitung persen alkohol berdasarkan persamaan kalibrasi
  float percent = (voltage - c) / m;

  // Hindari nilai negatif
  if (percent < 0) percent = 0;

  // Tampilkan hasil
  Serial.print("ADC: "); Serial.print(adcValue);
  Serial.print(" | Vout: "); Serial.print(voltage, 3);
  Serial.print(" V | Alkohol: "); Serial.print(percent, 2);
  Serial.println(" %");

  delay(1000);
}
