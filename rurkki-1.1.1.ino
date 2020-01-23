#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h> //NFC reader

#define SS_PIN 10 //SDA
#define RST_PIN 9 //RST

#define GREEN_LED 5
#define RED_LED 6

#define BUZZER_PIN 2

#define DOOR_SENSOR A0
int doorSensorStatus = LOW;

#define PIR_SENSOR 4
int pirSensorStatus = LOW;


int counter = 0;



  unsigned long lastTimePirTriggerd = 0;
  unsigned long lastTimeKey = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

String tulostaUID()
{
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;


  lastTimeKey = millis();

  noTone(BUZZER_PIN);


  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  return content;
}

void setup()
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Reader is ready...");

  Serial.println("rurkki-1.1.0-beta testaus 0.8");

  Serial.println();
  

  pinMode(DOOR_SENSOR, INPUT); //Asetetaan DOOR_SENSOR lukemaan inputtia oven statuksesta
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(PIR_SENSOR, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}


void loop()
{


  pirSensorStatus = digitalRead(PIR_SENSOR);

  if (pirSensorStatus == HIGH)
  {
    lastTimePirTriggerd = millis();
    Serial.println("PIR HIGH");
    Serial.println("---------");
  }

  unsigned long time = millis();
  const unsigned long halfMinutes = 0.5 * 60 * 1000;
  static unsigned long lastSampleTime = 0 - halfMinutes;
  if (time - lastSampleTime >= halfMinutes)
  { //If Half minute is passed, do the desired operations
  }

  doorSensorStatus = digitalRead(DOOR_SENSOR);


  if (doorSensorStatus == HIGH)
  {
            Serial.println("SENSOR HIGH");
  }
  

  if (millis() - lastTimePirTriggerd <= 30000)
  {
    if (doorSensorStatus == HIGH && millis() - lastTimeKey >= 30000)
    {
      tone(BUZZER_PIN, 1000, 300000L); //Viimeinen lukema määrittelee minuutit kuinka kauan buzzer voi maksimissaan soida.


  delay(500);

      digitalWrite(RED_LED, HIGH);     //Sytytetään punainen led
      Serial.println("Ovi on auki");
    }
  }
  

  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return; //palataan jos ei uutta korttia
  }
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return; //palataan jos ei kortteja
  }

  String content = tulostaUID(); //kutsutaan methodi tulostaUID, mikä tulostaa UID ja tallentaa Stringin loop methodiin

  if (content.substring(1) == "BD AF 24 D9")
  {
    Serial.println("Authorized access");
    tone(BUZZER_PIN, 2000, 500);

    doorSensorStatus = LOW;
    doorSensorStatus = digitalRead(DOOR_SENSOR);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    Serial.println();
    delay(3000);
    digitalWrite(GREEN_LED, LOW);
  }

  else
  {
    Serial.println(" Access denied");
    delay(3000);
  }
}
