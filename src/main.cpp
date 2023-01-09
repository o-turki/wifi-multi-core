#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

#define SCK 18
#define MOSI 23
#define MISO 19
#define SS_PIN 2
#define RST_PIN 4

MFRC522 mfrc522(SS_PIN, RST_PIN);

const int buzzer = 5;

const char *ssid = "ETUDIANT_Plus";
const char *password = "EnetcomEtud";

TaskHandle_t Task0;
void Task0code(void *pvParameters);

String tagRFID = "";

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(buzzer, OUTPUT);

  SPI.begin(); // Init SPI bus
  delay(100);
  mfrc522.PCD_Init();
  xTaskCreatePinnedToCore(Task0code, "Task0", 10000, NULL, 1, &Task0, 0);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
    return;

  if (mfrc522.PICC_IsNewCardPresent())
  {
    if (mfrc522.PICC_ReadCardSerial())
    {
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);
      delay(100);
      digitalWrite(buzzer, HIGH);
      delay(100);
      digitalWrite(buzzer, LOW);

      tagRFID = "";
      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        tagRFID += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
      }

      Serial.println("> TAG RFID: " + tagRFID);

      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }
}

void Task0code(void *pvParameters)
{
  // WiFi.mode(WIFI_STA); // Optional
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(200);

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  for (;;)
  {
    delay(1);
  }
}

// xTaskCreatePinnedToCore(
//     Task0code, /* Task function. */
//     "Task0",   /* name of task. */
//     10000,     /* Stack size of task */
//     NULL,      /* parameter of the task */
//     1,         /* priority of the task */
//     &Task0,    /* Task handle to keep track of created task */
//     0);        /* pin task to core 0 */
