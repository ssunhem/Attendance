# Attendance
Student Attendance Collector for Receive Information from Student ID Card and Thai ID Card and send to the Faculty Server and SD Card

------------------------------------------------------------
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
    
------------------------------------------------------------
