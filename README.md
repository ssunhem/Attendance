# Attendance
Student Attendance Collector for Receive Information from Student ID Card and Thai ID Card and send to the Faculty Server and SD Card

*** This Project is authorized by Student Employment Program, Faculty of Engineering, Chulalongkorn University ***

------------------------------------------------------------
  **How does it work?**
    Firstly, Device tries initializing SD Card then connect to WLAN for receiving and sending the students' information. Next step, users put their Thai ID Card into THID V2.0 Module Slot then close the Student ID Card to RFID tag. All information will be saved into SD Card and they will be sent to the Database of Faculty (In this case, I applied GET METHOD for convenience. It needs to be developed in the future for more security)

------------------------------------------------------------
  **Development Information**
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
