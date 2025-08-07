const int mq3Pin = A0;     // Pin analog MQ-3
const float Vref = 5.0;    // Tegangan referensi ADC (5V untuk Arduino Uno)

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Baca nilai ADC
  int sensorValue = analogRead(mq3Pin);

  // Konversi ke tegangan (0 - 5V)
  float voltage = (sensorValue / 1023.0) * Vref;

  // Hitung kadar alkohol menggunakan regresi linear
  float alcoholPercent = (voltage-1.7913) / 0.0268;

  // Jika hasil negatif, tampilkan 0
  if (alcoholPercent < 0) alcoholPercent = 0;

  // Tampilkan hasil
  Serial.print("ADC: ");
  Serial.print(sensorValue);
  Serial.print(" | Tegangan: ");
  Serial.print(voltage, 2);
  Serial.print(" V | Alkohol: ");
  Serial.print(alcoholPercent, 2);
  Serial.println(" %");

  delay(1000);  // Baca setiap 1 detik
}
