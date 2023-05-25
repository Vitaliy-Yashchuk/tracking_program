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