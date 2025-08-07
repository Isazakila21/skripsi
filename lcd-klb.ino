#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // alamat, kolom, baris

void setup() {
  lcd.init();        // GANTI dari lcd.begin() ke lcd.init()
  lcd.backlight();   // Nyalakan lampu latar

  lcd.setCursor(0, 0);
  lcd.print("Hello Nur Aisya");

  lcd.setCursor(0, 1);
  lcd.print("Zakila");
}

void loop() {
}
