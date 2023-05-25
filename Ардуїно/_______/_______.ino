/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

const int LED_PIN_1 = 2;
const int LED_PIN_2 = 3;
const int LED_PIN_3 = 4;

int numCards = 0;
int **cardData = NULL;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    int uidSize = mfrc522.uid.size;
    byte *uid = mfrc522.uid.uidByte;

    int cardIndex = -1;
    for (int i = 0; i < numCards; i++) {
      if (compareArrays(cardData[i], uid, uidSize)) {
        cardIndex = i;
        break;
      }
    }

    if (cardIndex == -1) {
      // Add new card to array
      int **newData = new int*[numCards + 1];
      for (int i = 0; i < numCards; i++) {
        newData[i] = cardData[i];
      }
      newData[numCards] = new int[uidSize];
      for (int i = 0; i < uidSize; i++) {
        newData[numCards][i] = uid[i];
      }
      numCards++;

      delete[] cardData;
      cardData = newData;

      // Turn on LED corresponding to number of cards
      switch (numCards) {
        case 1:
          digitalWrite(LED_PIN_1, HIGH);
          break;
        case 2:
          digitalWrite(LED_PIN_2, HIGH);
          break;
        case 3:
          digitalWrite(LED_PIN_3, HIGH);
          break;
      }
    } else {
      // Remove card from array
      removeElement(cardData, numCards, cardIndex);
      numCards--;

      // Turn off LED corresponding to number of cards
      switch (numCards) {
        case 0:
          digitalWrite(LED_PIN_1, LOW);
          digitalWrite(LED_PIN_2, LOW);
          digitalWrite(LED_PIN_3, LOW);
          break;
        case 1:
          digitalWrite(LED_PIN_2, LOW);
          digitalWrite(LED_PIN_3, LOW);
          break;
        case 2:
          digitalWrite(LED_PIN_3, LOW);
          break;
      }
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}

bool compareArrays(int* arr1, byte* arr2, int size) {
  for (int i = 0; i < size; i++) {
    if (arr1[i] != arr2[i]) {
      return false;
    }
  }
  return true;
}

void removeElement(int** arr, int size, int index) {
  for (int i = index; i < size - 1; i++) {
    arr[i] = arr[i + 1];
  }
}





 
