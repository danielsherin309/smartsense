#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <time.h>

// ========================================
// Wi-Fi
// ========================================

#define WIFI_SSID "Daniel Sherin"
#define WIFI_PASSWORD "daniel1178"


// ========================================
// Firebase
// ========================================

#define API_KEY "AIzaSyB5YljHzNbJaqtDxxNyuHks-H62lnqmbYY"

#define DATABASE_URL "https://smartsense-743cc-default-rtdb.asia-southeast1.firebasedatabase.app"


// ========================================
// Firebase Login
// ========================================

#define USER_EMAIL "smartsense.device@gmail.com"
#define USER_PASSWORD "smart123"


// ========================================
// DHT11
// ========================================

#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);


// ========================================
// Firebase objects
// ========================================

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


// ========================================
// Simulated sensor values
// ========================================

int lightLevel;
bool motionDetected;
int airQuality;


// ========================================
// Alert control
// ========================================

bool temperatureAlertSent = false;
bool humidityAlertSent = false;
bool airQualityAlertSent = false;
bool motionAlertSent = false;


// ========================================
// IST TIME
// ========================================

const char* ntpServer = "pool.ntp.org";

const long gmtOffset_sec = 19800;

const int daylightOffset_sec = 0;


// ========================================
// GET CURRENT TIME
// ========================================

String getCurrentTime() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {

    return "TIME_NOT_AVAILABLE";
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


// ========================================
// GET UNIX TIMESTAMP
// ========================================

unsigned long getUnixTimestamp() {

  time_t now;

  time(&now);

  return (unsigned long)now;
}


// ========================================
// SAVE ALERT
// ========================================

void saveAlert(
  String type,
  String message,
  float value
) {

  FirebaseJson alertData;

  alertData.set("type", type);

  alertData.set("message", message);

  alertData.set("value", value);

  alertData.set(
    "timestamp",
    getCurrentTime()
  );

  alertData.set(
    "timestampUnix",
    getUnixTimestamp()
  );


  if (Firebase.RTDB.pushJSON(
        &fbdo,
        "/devices/ESP32_01/alerts",
        &alertData
      )) {

    Serial.println();

    Serial.println("🚨 ALERT SAVED!");

    Serial.print("Type    : ");

    Serial.println(type);

    Serial.print("Message : ");

    Serial.println(message);

    Serial.print("Value   : ");

    Serial.println(value);

    Serial.print("Time    : ");

    Serial.println(getCurrentTime());

  }

  else {

    Serial.print("Alert save failed: ");

    Serial.println(fbdo.errorReason());
  }
}


// ========================================
// SETUP
// ========================================

void setup() {

  Serial.begin(115200);

  delay(1000);


  Serial.println();

  Serial.println("================================");

  Serial.println("       SMARTSENSE FIREBASE");

  Serial.println("================================");


  // ------------------------------------
  // Start DHT11
  // ------------------------------------

  dht.begin();

  Serial.println("DHT11 initialized");


  // ------------------------------------
  // Connect Wi-Fi
  // ------------------------------------

  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }


  Serial.println();

  Serial.println("Wi-Fi Connected!");


  Serial.print("IP Address: ");

  Serial.println(
    WiFi.localIP()
  );


  // ====================================
  // START NTP
  // ====================================

  Serial.println();

  Serial.println("Synchronizing time...");


  configTime(
    gmtOffset_sec,
    daylightOffset_sec,
    ntpServer
  );


  struct tm timeinfo;


  if (getLocalTime(&timeinfo)) {

    Serial.println("Time synchronized!");

    Serial.print("Current IST time: ");

    Serial.println(
      getCurrentTime()
    );

  }

  else {

    Serial.println(
      "Time synchronization failed!"
    );
  }


  // ====================================
  // FIREBASE
  // ====================================

  config.api_key = API_KEY;

  config.database_url = DATABASE_URL;


  auth.user.email = USER_EMAIL;

  auth.user.password = USER_PASSWORD;


  Firebase.begin(
    &config,
    &auth
  );

  Firebase.reconnectWiFi(true);


  Serial.println(
    "Firebase initialized"
  );
}


// ========================================
// LOOP
// ========================================

void loop() {


  // ======================================
  // READ DHT11
  // ======================================

  float temperature =
    dht.readTemperature();


  float humidity =
    dht.readHumidity();


  if (
    isnan(temperature) ||
    isnan(humidity)
  ) {

    Serial.println(
      "DHT11 reading failed"
    );

    delay(2000);

    return;
  }


  // ======================================
  // SIMULATED VALUES
  // ======================================

  lightLevel =
    random(20, 101);


  motionDetected =
    random(0, 2);


  airQuality =
    random(40, 101);


  // ======================================
  // SERIAL OUTPUT
  // ======================================

  Serial.println();

  Serial.println(
    "----------- SENSOR DATA -----------"
  );


  Serial.print(
    "Time        : "
  );

  Serial.println(
    getCurrentTime()
  );


  Serial.print(
    "Temperature : "
  );

  Serial.print(
    temperature
  );

  Serial.println(
    " °C"
  );


  Serial.print(
    "Humidity    : "
  );

  Serial.print(
    humidity
  );

  Serial.println(
    " %"
  );


  Serial.print(
    "Light Level : "
  );

  Serial.println(
    lightLevel
  );


  Serial.print(
    "Motion      : "
  );


  if (motionDetected) {

    Serial.println(
      "DETECTED"
    );

  }

  else {

    Serial.println(
      "NOT DETECTED"
    );
  }


  Serial.print(
    "Air Quality : "
  );

  Serial.println(
    airQuality
  );


  // ======================================
  // FIREBASE
  // ======================================

  if (Firebase.ready()) {


    // ====================================
    // CURRENT VALUES
    // ====================================

    Firebase.RTDB.setFloat(
      &fbdo,
      "/devices/ESP32_01/temperature",
      temperature
    );


    Firebase.RTDB.setFloat(
      &fbdo,
      "/devices/ESP32_01/humidity",
      humidity
    );


    Firebase.RTDB.setInt(
      &fbdo,
      "/devices/ESP32_01/light",
      lightLevel
    );


    Firebase.RTDB.setBool(
      &fbdo,
      "/devices/ESP32_01/motion",
      motionDetected
    );


    Firebase.RTDB.setInt(
      &fbdo,
      "/devices/ESP32_01/airQuality",
      airQuality
    );


    Serial.println(
      "Current values uploaded!"
    );


    // ====================================
    // HISTORY
    // ====================================

    FirebaseJson historyData;


    historyData.set(
      "temperature",
      temperature
    );


    historyData.set(
      "humidity",
      humidity
    );


    historyData.set(
      "light",
      lightLevel
    );


    historyData.set(
      "motion",
      motionDetected
    );


    historyData.set(
      "airQuality",
      airQuality
    );


    historyData.set(
      "timestamp",
      getCurrentTime()
    );


    historyData.set(
      "timestampUnix",
      getUnixTimestamp()
    );


    if (
      Firebase.RTDB.pushJSON(
        &fbdo,
        "/devices/ESP32_01/history",
        &historyData
      )
    ) {

      Serial.println(
        "History saved!"
      );

    }

    else {

      Serial.print(
        "History upload failed: "
      );

      Serial.println(
        fbdo.errorReason()
      );
    }


    // ====================================
    // TEMPERATURE ALERT
    // ====================================

    if (temperature >= 35) {

      if (!temperatureAlertSent) {

        saveAlert(
          "HIGH_TEMPERATURE",
          "Temperature is too high",
          temperature
        );

        temperatureAlertSent = true;
      }

    }

    else {

      temperatureAlertSent = false;
    }


    // ====================================
    // HUMIDITY ALERT
    // ====================================

    if (humidity >= 80) {

      if (!humidityAlertSent) {

        saveAlert(
          "HIGH_HUMIDITY",
          "Humidity is too high",
          humidity
        );

        humidityAlertSent = true;
      }

    }

    else {

      humidityAlertSent = false;
    }


    // ====================================
    // AIR QUALITY ALERT
    // ====================================

    if (airQuality <= 50) {

      if (!airQualityAlertSent) {

        saveAlert(
          "POOR_AIR_QUALITY",
          "Air quality is poor",
          airQuality
        );

        airQualityAlertSent = true;
      }

    }

    else {

      airQualityAlertSent = false;
    }


    // ====================================
    // MOTION ALERT
    // ====================================

    if (motionDetected) {

      if (!motionAlertSent) {

        saveAlert(
          "MOTION_DETECTED",
          "Motion detected",
          1
        );

        motionAlertSent = true;
      }

    }

    else {

      motionAlertSent = false;
    }

  }

  else {

    Serial.println(
      "Firebase not ready"
    );
  }


  // ======================================
  // WAIT 5 SECONDS
  // ======================================

  delay(5000);
}