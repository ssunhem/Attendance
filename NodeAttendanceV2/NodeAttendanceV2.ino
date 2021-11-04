//  created  Sep 2020
//  by Rew INTANIA103
//  for more information contact rewlgil@hotmail.com

/*------------------------------------------------------------
  developed  Aug 2021
  by Sakunthan Sunhem INTANIA103
  for more information, email:ssunhem@gmail.com, line:ssunhem
------------------------------------------------------------*/

/*------------------------------------------------------------
  Development Information
  Engineering Attendance Kit was created by applying RFID communication,
  In this version, Reading Thai National ID Card and communication with a server are available

  Pins connection:
  1. SD Card Reader --> ESP32 Ayarafun (Use ESP32 Dev Module in Board Manager)
    GND   --> GND
    +3.3  --> 3v3
    CS    --> 14
    MOSI  --> 23
    SCK   --> 18
    MISO  --> 19
  2. RC522(RFID Reader) --> ESP32 Ayarafun
    NSS   --> 12
    SCK   --> 18  (SPI communication: Connect parallel with SD Card Reader)
    MOSI  --> 23  (SPI communication: Connect parallel with SD Card Reader)
    MISO  --> 19  (SPI communication: Connect parallel with SD Card Reader)
    RST   --> 13
    GND   --> GND
    3.3V  --> 3v3
  3. LCD --> ESP32 Ayarafun
    GND   --> GND
    VCC   --> RAW
    SDA   --> 21
    SCL   --> 22
  4. THID v2.0 --> ESP32 Ayarafun
    GND   --> GND
    TX    --> 16 (RX pin of Serial 2 of ESP32)
    RX    --> 17 (TX pin of Serial 2 of ESP32)
    VCC   --> RAW
------------------------------------------------------------*/

// Import all important libraries
// ------------------------------------------------------------
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include "FS.h"
#include <SD.h>

// Define all needed Pins
// ------------------------------------------------------------

// Pins for RFID Reader
#define SS_PIN  12
#define RST_PIN 13

// Pin for SD
#define CS_PIN 14

// Pins for THID V2.0
#define RX2 16
#define TX2 17

// Declare all important objects and variables
// ------------------------------------------------------------
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
LiquidCrystal_I2C lcd(0x3f, 16, 2);
WiFiManager wifiManager;
HTTPClient httpClient;
File dataFile;

String url = "https://student.eng.chula.ac.th/app/checkrfid.php?rfid=";     // Depend on Server data storage

String dirName = "/Attendance";
String fileName = "/ID";
String newFile = "";

String thID = "";
char readbyte = 0;

String strID = "";
uint32_t lastread = 0;
bool cleared = true;
bool online = false;

String projName = "";

String readtag(void);
// ------------------------------------------------------------

void setup() {
  Serial.begin(115200);     // Connect to Serial Monitor
  Serial2.begin(115200,SERIAL_8N1,RX2,TX2);     // Connect to THID V2.0 Module
//  wifiManager.resetSettings();
  lcd.begin();
  lcd.backlight();


// Connect to SD Card
// ------------------------------------------------------------
  Serial.println("Initializing SD card...");
  lcd.home();
  lcd.print("checking SD card");
  lcd.home();
  delay(1000);
  while (!SD.begin(CS_PIN))
  {
  }
  Serial.println("card initialized.");
  
  File conf = SD.open("/cf.txt");
  while(!conf){}
    while(conf.available()){
      projName += conf.readString();
    }
  conf.close();
  Serial.print("Project Name: ");
  Serial.println(projName);

  Serial.println("Creating Dir: /Attendance");
  if(!SD.mkdir(dirName))
  {
    Serial.println("mkdir failed");
    lcd.home();
    lcd.print("mkdir failed    ");
    while(1);
  }
  Serial.println("Dir created");
  lcd.home();
  lcd.print("Dir created     ");

  File dir = SD.open(dirName);
  if(!dir){
      Serial.println("Failed to open directory");
      while(1);
  }

  String number;
  uint8_t lastFile = 0;
  
  File file = dir.openNextFile();
  while(file)
  {
    String _fileName = file.name();
    Serial.println(_fileName);
    
    if (_fileName.substring(_fileName.lastIndexOf(".")) == ".TXT" &&
        _fileName.substring(_fileName.lastIndexOf("/"), 
                            _fileName.lastIndexOf("/") + fileName.length()) == fileName)
    {
      number = _fileName.substring(_fileName.lastIndexOf("/") + fileName.length(),
                                   _fileName.lastIndexOf("."));
                                   
      if (number.toInt() > lastFile)
        lastFile = number.toInt();
    }
    file = dir.openNextFile();
  }
  file.close();
  dir.close();

fileName.concat((lastFile + 1) < 10 ? "0" : "");
  fileName.concat(lastFile + 1);
  fileName.concat(".TXT");
  newFile = dirName + fileName;

  file = SD.open(newFile, FILE_WRITE);
  if(!file)
  {
    Serial.println("Failed to open file for writing");
  }
  file.close();
  Serial.print("File created: ");   Serial.println(newFile);
  lcd.home();           lcd.print("File created!!! ");
  delay(2000);          lcd.clear();
  lcd.home();           lcd.print(dirName);
  lcd.setCursor(0, 1);  lcd.print(fileName);

// Connect to RFID Reader
// ------------------------------------------------------------
  SPI.begin();
  rfid.PCD_Init();

  delay(5000);          lcd.clear();
  lcd.home();           lcd.print("connecting Wi-Fi");

// Connect to WLAN
// ------------------------------------------------------------
  wifiManager.setTimeout(60);
  wifiManager.setAPCallback(configModeCallback);
  if(! wifiManager.autoConnect("card_reader")) 
  {
    Serial.println("failed to connect and hit timeout");
    online = false;
    lcd.clear();
    lcd.home();           lcd.print("offline mode    ");
  }
  else
  {
    online = true;
    lcd.clear();
    lcd.home();           lcd.print("connected to : ");
    lcd.setCursor(0, 1);  lcd.print(WiFi.SSID());
  }
  
  Serial.print("\n\n");
  delay(5000);            lcd.clear();
  lcd.home();             lcd.print("insert THID card");
}
void loop() 
{
  if(Serial2.available()>0){        // Read the information on Thai ID Card
    readbyte = Serial2.read();
    if(readbyte!='\r'){
      thID += readbyte;
    }
  }
  if(readbyte==0x0D){
    if (thID != "REMOVE"){
      lcd.home(); lcd.print("ID:");
      lcd.setCursor(3,0); lcd.print(thID);
      lcd.setCursor(0,1); lcd.print("tab to check-in");
      strID = readtag();
      if (strID != "")
      {
        Serial.println(strID);
        Serial.println(thID);
        if (online)
        {
          url.concat(strID);
          Serial.println("url = ");       Serial.println(url.c_str());
          httpClient.begin(url.c_str());
          int httpCode = httpClient.GET();
          
          if( httpCode > 0 ) 
          {
            String response = httpClient.getString();     // Get Student ID from above URL
            Serial.print("response: "); Serial.println(response);
            httpClient.end();

            // Create URL for sending information
            String serverURL = "https://192.168.1.108:5000/?"; // depend on which server are serving for
            String fsParam = "stdID=";
            String ndParam = "&thID=";
            String rdParam = "&proj=";
            String allParam = serverURL+fsParam+response+ndParam+thID/*+rdParam+projName*/;
            Serial.println(allParam);
            httpClient.begin(allParam);
            //httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
            int httpCode2 = httpClient.GET();
            Serial.println(httpCode2);

            // Add information into File
            if (response == "0")
            {
              lcd.home();               lcd.print("  ---UNKNOW---  ");
              lcd.setCursor(0, 1);      lcd.print("                ");
              lcd.setCursor(0, 1);      lcd.print(strID);
              appendFile(strID + " " + thID + " " + projName + "\n");
            }
            else
            {
              lcd.home();               lcd.print("    WELCOME     ");
              lcd.setCursor(0, 1);      lcd.print("                ");
              lcd.setCursor(0, 1);      lcd.print(response);
              appendFile(response + " " + thID + " " + projName + "\n");
            }
          }
          else
          {
            Serial.printf("error HTTP code: %d\n", httpCode);
            lcd.clear();
            lcd.home();               lcd.print("error ");              lcd.print(httpCode);
            lcd.setCursor(0, 1);      lcd.print("                ");
            lcd.setCursor(0, 1);      lcd.print(strID);
            appendFile(strID + " " + thID + " " + projName + "\n");
          }
        }
        else
        {
          lcd.home();               lcd.print("  ---offline--- ");
          lcd.setCursor(0, 1);      lcd.print("                ");
          lcd.setCursor(0, 1);      lcd.print(strID);
          appendFile(strID + " " + thID + " " + projName + "\n");
        }
        delay(1000);
        lastread = millis();
        cleared = false;
      }
      if (millis() - lastread >= 4000 && cleared == false)
      {
        lcd.home();             lcd.print("remove THID card");
        cleared = true;
        httpClient.end();
      }
    }
    else {
      lcd.clear(); lcd.home();
      lcd.setCursor(0,0); lcd.print("insert THID card");
    }
//  count = 0;
  }
}

void configModeCallback (WiFiManager *mywifiManager){
  lcd.home();           lcd.print("Starting AP...  ");
  lcd.setCursor(0, 1);  lcd.print("AP : card_reader");
}

String readtag(void){
  String strID = "";

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
  {
//    Serial.print("PICC type: ");
//    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
//    Serial.println(rfid.PICC_GetTypeName(piccType));

    for (byte i = 0; i < 7; i++)
    {
      strID += (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
         String(rfid.uid.uidByte[i], HEX);
    }
    strID.toUpperCase();
//    Serial.println(strID);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  return strID;
}

void appendFile(String message){
    Serial.println("Appending to file");

    File file = SD.open(newFile, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending\n");
        lcd.home();       lcd.print("SD card failed  ");
        return;
    }
    if(file.print(message))
        Serial.println("Message appended\n");
    else
    {
        Serial.println("Append failed\n");
        lcd.home();       lcd.print("Append failed   ");
        cleared = false;
        lastread = millis();
    }
    file.close();
}
