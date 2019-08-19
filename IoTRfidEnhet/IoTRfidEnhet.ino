#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <SPI.h>
#include <MFRC522.h>

//RFID Reader
#define RST_PIN 26
#define SS_PIN  14
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//HMI
int freq = 2000;
int channel = 0;
int resolution = 8;
int redLed = 15;
int greenLed = 32;

//Database
IPAddress server_addr(10,0,0,62);  // IP of the MySQL *server* here
char user[] = "RfidReader1";              // MySQL user login username
char password[] = "fall2019";     // MySQL user login password

// Query
char query[] = "SELECT personId, firstName FROM `satelliteaccesscontrol`.person;";

// WiFi
char ssid[] = "Rhadowsnett";        // your SSID
char pass[] = "Narnia123";       // your SSID Password
WiFiClient client;

MySQL_Connection conn((Client *)&client);
// Create an instance of the cursor passing in the connection
MySQL_Cursor cur = MySQL_Cursor(&conn);

void setup() {
  
  Serial.begin(9600);
  while (!Serial); // wait for serial port to connect.

  SPI.begin();
  mfrc522.PCD_Init();
  
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(33, OUTPUT);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(33, channel);

  // Connect to WiFi
  Serial.println("Connecting to network");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  // print out info about the connection:
    Serial.println("\nConnected to network");
    Serial.print("My IP address is: ");
    Serial.println(WiFi.localIP());
    Serial.print("My MAC address is: ");
    Serial.println(WiFi.macAddress());
  //Connect to Database
  Serial.println("Connecting to database");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else{
    Serial.println("Connection failed.");
    }

  delay(1000);
}

void loop() {
  // Prepare key (FFFFFFFFFFFFh)
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  byte block;
  byte len;
  MFRC522::StatusCode status;

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card to Serial

  byte byteBuffer[18];
  block = 1;
  len = 18;

  // Autorization of block 1
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); 
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    accessDenied();
    return;
  }

  Serial.print(F("Person ID: "));
  
  //Read personId from block 1
  status = mfrc522.MIFARE_Read(block, byteBuffer, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Convert personId to int
  int personId = convertToInt(byteBuffer);
  Serial.println(personId);

  //Authorize person in database
  bool pInDb = personIdInDatabase(personId);
  if(pInDb){
    accessGranted();
  }
  else{
    accessDenied();
  }
  delay(1000); //Change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
