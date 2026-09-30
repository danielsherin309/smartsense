#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <time.h>
#include "config.h"

// ================= DHT11 =================
#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// ================= FIREBASE =================
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ================= TIME =================
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

// ================= SENSOR VALUES =================
float temperature = 0;
float humidity = 0;

int lightLevel = 0;
bool motionDetected = false;
int airQuality = 0;

// ================= ALERT FLAGS =================
bool temperatureAlert = false;
bool humidityAlert = false;
bool airQualityAlert = false;

// ================= TIME FUNCTION =================
String getCurrentTime() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    return "Time unavailable";
  }

  char timeString[30];

  strftime(
    timeString,
    sizeof(timeString),
    "%Y-%m-%d %H:%M:%S",
    &timeinfo
  );

  return String(timeString);
}

// ================= UNIX TIMESTAMP =================
unsigned long getUnixTimestamp() {
  time_t now;

  time(&now);

  return (unsigned long)now;
}

// ================= SAVE ALERT =================
void saveAlert(String type, String message, float value) {

  if (!Firebase.ready()) {
    return;
  }

  FirebaseJson json;

  json.set("type", type);
  json.set("message", message);
  json.set("value", value);
  json.set("timestamp", getUnixTimestamp());
  json.set("time", getCurrentTime());

  String path = "/devices/ESP32_01/alerts";

  if (Firebase.RTDB.pushJSON(&fbdo, path, &json)) {

    Serial.println("Alert saved to Firebase");

  } else {

    Serial.print("Alert save failed: ");
    Serial.println(fbdo.errorReason());
  }
}

// ================= SETUP =================
void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("=================================");
  Serial.println("        SMARTSENSE ESP32");
  Serial.println("=================================");

  // Start DHT
  dht.begin();

  Serial.println("DHT11 initialized");

  // ================= WIFI =================
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ================= TIME =================
  configTime(
    gmtOffset_sec,
    daylightOffset_sec,
    "pool.ntp.org",
    "time.nist.gov"
  );

  Serial.println("Time synchronization started");

  // ================= FIREBASE =================
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized");

  Serial.println();
  Serial.println("SmartSense is ready!");
  Serial.println();
}

// ================= LOOP =================
void loop() {

  // ================= READ DHT11 =================
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {

    Serial.println("Failed to read DHT11!");

    delay(2000);

    return;
  }

  // ================= SIMULATED SENSORS =================
  lightLevel = random(20, 100);

  motionDetected = random(0, 2);

  airQuality = random(40, 100);

  // ================= SERIAL OUTPUT =================
  Serial.println("---------------------------------");
  Serial.println("        SENSOR DATA");
  Serial.println("---------------------------------");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Light Level : ");
  Serial.println(lightLevel);

  Serial.print("Motion      : ");

  if (motionDetected) {
    Serial.println("DETECTED");
  } else {
    Serial.println("NOT DETECTED");
  }

  Serial.print("Air Quality : ");
  Serial.println(airQuality);

  Serial.print("Time        : ");
  Serial.println(getCurrentTime());

  // ================= FIREBASE =================
  if (Firebase.ready()) {

    String basePath = "/devices/ESP32_01/current";

    Firebase.RTDB.setFloat(
      &fbdo,
      basePath + "/temperature",
      temperature
    );

    Firebase.RTDB.setFloat(
      &fbdo,
      basePath + "/humidity",
      humidity
    );

    Firebase.RTDB.setInt(
      &fbdo,
      basePath + "/lightLevel",
      lightLevel
    );

    Firebase.RTDB.setBool(
      &fbdo,
      basePath + "/motionDetected",
      motionDetected
    );

    Firebase.RTDB.setInt(
      &fbdo,
      basePath + "/airQuality",
      airQuality
    );

    Firebase.RTDB.setString(
      &fbdo,
      basePath + "/timestamp",
      getCurrentTime()
    );

    // ================= HISTORY =================
    FirebaseJson historyData;

    historyData.set("temperature", temperature);
    historyData.set("humidity", humidity);
    historyData.set("lightLevel", lightLevel);
    historyData.set("motionDetected", motionDetected);
    historyData.set("airQuality", airQuality);
    historyData.set("timestamp", getUnixTimestamp());
    historyData.set("time", getCurrentTime());

    Firebase.RTDB.pushJSON(
      &fbdo,
      "/devices/ESP32_01/history",
      &historyData
    );

    // ================= TEMPERATURE ALERT =================
    if (temperature > 35 && !temperatureAlert) {

      saveAlert(
        "Temperature",
        "High temperature detected",
        temperature
      );

      temperatureAlert = true;
    }

    if (temperature <= 35) {
      temperatureAlert = false;
    }

    // ================= HUMIDITY ALERT =================
    if (humidity > 80 && !humidityAlert) {

      saveAlert(
        "Humidity",
        "High humidity detected",
        humidity
      );

      humidityAlert = true;
    }

    if (humidity <= 80) {
      humidityAlert = false;
    }

    // ================= AIR QUALITY ALERT =================
    if (airQuality < 50 && !airQualityAlert) {

      saveAlert(
        "Air Quality",
        "Poor air quality detected",
        airQuality
      );

      airQualityAlert = true;
    }

    if (airQuality >= 50) {
      airQualityAlert = false;
    }

    Serial.println("Firebase updated successfully");
  }

  Serial.println();

  delay(5000);
}
