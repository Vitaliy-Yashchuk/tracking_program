#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

const int maxSeats = 4; 
int freeSeats = maxSeats; 
String uids[maxSeats]; 
int currentIndex = 0; 

void setup() {
  
  lcd.begin(16, 2); 
  lcd.backlight(); 
  SPI.begin();
  rfid.PCD_Init();
  
  lcd.print("Free Seats: 4");
}

void loop() {

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uid += String(rfid.uid.uidByte[i], HEX);
    }

    bool found = false;
    for (int i = 0; i < currentIndex; i++) {
      if (uids[i] == uid) {
        found = true;

        for (int j = i; j < currentIndex - 1; j++) {
          uids[j] = uids[j + 1];
        }
        currentIndex--;
        freeSeats++;
        break;
      }
    }

    if (!found && currentIndex < maxSeats) {
      uids[currentIndex] = uid;
      currentIndex++;
      freeSeats--;
    }

    lcd.clear();
    lcd.print("Free seats: ");
    lcd.print(freeSeats);
    delay(1000);
    rfid.PICC_HaltA();
  }
}
