//code for master: 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// TGAM EEG on Serial1 (RX1=19, TX1=18)
// HC-05 Master on Serial2 (RX2=17, TX2=16)
LiquidCrystal_I2C lcd(0x27, 16, 2);
int attention = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);   // TGAM EEG
  Serial2.begin(38400);  // HC-05 Master

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("EEG Transmitter");
}

void loop() {
  static byte lastByte = 0;
  static byte payload[32];
  static int payloadIndex = 0;
  static int payloadLength = 0;
  static bool syncFound = false;

  while (Serial1.available()) {
    byte currentByte = Serial1.read();

    if (!syncFound) {
      if (lastByte == 0xAA && currentByte == 0xAA) {
        syncFound = true;
        payloadIndex = 0;
      }
      lastByte = currentByte;
      continue;
    }

    if (payloadIndex == 0) {
      payloadLength = currentByte;
      if (payloadLength > sizeof(payload)) {
        syncFound = false;
        lastByte = 0;
        continue;
      }
      payloadIndex++;
      continue;
    }

    if (payloadIndex <= payloadLength) {
      payload[payloadIndex - 1] = currentByte;
      payloadIndex++;
      continue;
    }

    byte checksum = currentByte;
    byte sum = 0;
    for (int i = 0; i < payloadLength; i++) sum += payload[i];
    sum = 255 - sum;

    if (sum == checksum) {
      for (int i = 0; i < payloadLength - 1; i++) {
        if (payload[i] == 0x04) {
          attention = payload[i + 1];
          Serial.print("Attention: ");
          Serial.println(attention);

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Attention:");
          lcd.setCursor(0, 1);
          lcd.print(attention);

          // Send via Bluetooth
          Serial2.print("ATT:");
          Serial2.println(attention);
          break;
        }
      }
    }

    syncFound = false;
    lastByte = 0;
  }
}