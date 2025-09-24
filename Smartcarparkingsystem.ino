#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 53 
#define RST_PIN 5   
MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo gateServo;
Servo myServo;


int IR1 = 2;     
int IR2 = 3;  
int buzzer = 6;
int led = 13;

int Slot = 4;        
int flag1 = 0;
int flag2 = 0;

String allowedCards[] = {
  "F4D21205",   
  "0362CDA9"   
};
int totalCards = 2;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();
  lcd.backlight();

  gateServo.attach(4);
  gateServo.write(100);

  
  myServo.attach(9);
  myServo.write(100);   

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  lcd.setCursor(0,0);
  lcd.print("    ARDUINO    ");
  lcd.setCursor(0,1);
  lcd.print(" PARKING SYSTEM  ");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String readUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10) readUID += "0";
      readUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    readUID.toUpperCase();

    Serial.print("Scanned UID: ");
    Serial.println(readUID);

    if (checkUID(readUID)) {
      if (Slot > 0) {           
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Access Granted");
        myServo.write(55);      
        delay(2000);
        myServo.write(100);   
      } else {
        fullAlert();
      }
    } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Access Denied");
      delay(2000);
    }

    mfrc522.PICC_HaltA();
  }

  if (digitalRead(IR1) == LOW && flag1 == 0) {
    if (Slot > 0) {
      flag1 = 1;
      if (flag2 == 0) {
        gateServo.write(80); 
        Slot--;
      }
    } else {
      fullAlert();
    }
  }

  if (digitalRead(IR2) == LOW && flag2 == 0) {
    flag2 = 1;
    if (flag1 == 0) {
      gateServo.write(80);
      Slot++;
    }
  }


  
  if (flag1 == 1 && flag2 == 1) {
    delay(1000);
    gateServo.write(100); 
    flag1 = 0;
    flag2 = 0;
  }

  lcd.setCursor(0,0);
  lcd.print("   WELCOME!    ");
  lcd.setCursor(0,1);
  lcd.print(" Slots Left: ");
  lcd.print(Slot);
}

bool checkUID(String uid) {
  for (int i = 0; i < totalCards; i++) {
    if (uid == allowedCards[i]) return true;
  }
  return false;
}

void fullAlert() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("     SORRY!     ");
  lcd.setCursor(0,1);
  lcd.print("  Parking Full  ");
  delay(2000);
  lcd.clear();

  for(int i=0;i<10;i++){ 
    digitalWrite(buzzer,HIGH);
    digitalWrite(led,HIGH);
    delay(200);
    digitalWrite(buzzer,LOW);
    digitalWrite(led,LOW);
    delay(200);
  }
}
