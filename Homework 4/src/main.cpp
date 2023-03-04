#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "SensorData.h"

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Ultrasonic sensor
#define echo 14
#define trigger 12
#define ultrasonicDivisor 58
uint8_t distance = 0;
unsigned long t = 0;


#define DHTPIN 4          // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);

// Insert your network credentials
#define WIFI_SSID "<YOUR_SSID>"
#define WIFI_PASSWORD "<YOUR_PASSWORD>"

// Insert Firebase project API Key
#define API_KEY "<YOUR_API_KEY>"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "<YOUR_USER>"
#define USER_PASSWORD "<YOUR_USER_PASSWORD>"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "<YOUR_DATABASE_URL>"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Variables to save database paths
String databasePath;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 5000;

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Write float values to the database
void sendJson(FirebaseJson firebaseJson){
  if (Firebase.RTDB.setJSON(&fbdo, static_cast<const char*>(databasePath.c_str()), &firebaseJson)) {
      Serial.println("Upload success");
    } else {
      Serial.println("Upload failed");
      Serial.println(fbdo.errorReason());
    }
}

void setup(){
  Serial.begin(115200);

  // Initialize BME280 sensor
  //initBME();
  initWiFi();

  // Init DHT
  dht.begin();

  // Init ultrasonic
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid;
}

void loop(){
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    SensorData sensorData;
    sensorData.Temperature = dht.readTemperature();
    sensorData.Humidity = dht.readHumidity();

    // Distance
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);

    t = pulseIn(echo, HIGH);
    sensorData.Distance = t/ultrasonicDivisor;
    Serial.print("The distance is: ");
    Serial.println(sensorData.Distance);

    // Create a JSON object and serialize the SensorData instance
    StaticJsonDocument<200> doc;
    doc["humidity"] = sensorData.Humidity;
    doc["temperature"] = sensorData.Temperature;
    doc["distance"] = sensorData.Distance;

    String jsonString;
    serializeJson(doc, jsonString);

    Serial.print("Temperature: ");
    Serial.print(sensorData.Temperature);
    Serial.print(" °C\t");
    Serial.print("Humidity: ");
    Serial.print(sensorData.Humidity);
    Serial.println(" %");

    // Create a FirebaseJson object and copy the JSON object into it
    FirebaseJson firebaseJson;
    firebaseJson.setJsonData(jsonString.c_str());

    // Upload the JSON object to Firebase
    sendJson(firebaseJson);
  }
}
