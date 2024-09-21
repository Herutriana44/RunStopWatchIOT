#include <WiFi.h>
#include <ESP32Firebase.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Konfigurasi WiFi
#define WIFI_SSID "HERU TRIANA X250"
#define WIFI_PASSWORD "heru1234"

#define INTERNAL_LED 2

// Konfigurasi Firebase
#define REFERENCE_URL "https://runningstopwatch-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyBf4pnbvP_XMgu7edV6zw-cyEzRqC5l1_I"
#define FIREBASE_DB_SECRET "35go3AJ3TO1Ke6oMvs0fm6NmkVRpE6uqEFjGZmPE"
Firebase firebase(REFERENCE_URL);
// Pin untuk sensor inframerah
const int buttonPin = 26;
int buttonState = 0;

// Buat objek Firebase
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig config;

void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // // firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, FIREBASE_DB_SECRET);
  // config.database_url = DATABASE_URL;
  // config.api_key = FIREBASE_AUTH;
  // firebase.begin(&config, &auth);
  // firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  // firebase.reconnectWiFi(true);

  pinMode(buttonPin, INPUT);
  pinMode(INTERNAL_LED,OUTPUT);
  Serial.println("Program dimulai");
}

void loop() {
  buttonState = digitalRead(buttonPin);
  // Baca nilai sensor inframerah (Anda perlu menyesuaikan fungsi ini sesuai dengan sensor Anda)
  // Misalnya, jika nilai analog lebih besar dari nilai ambang batas, berarti ada objek
  if (buttonState == HIGH) {
    Serial.println("Objek terdeteksi!");
    digitalWrite(INTERNAL_LED,HIGH);
    
    // Kirim data ke Firebase
    // firebase.setJSON(fbdo, "/data", "{\"start\":true, \"finish\":false}");
    if (firebase.setInt("start", 1)) {
      Serial.println("Set successful");
    } else {
      Serial.println("Set failed");
      // Serial.println(fbdo.errorReason());
    }

    // // Delay 3 detik
    // delay(1000);
  } else {
    Serial.println("Objek tidak terdeteksi!");
    digitalWrite(INTERNAL_LED,LOW);
    // if (firebase.setInt("start", 0)) {
    //  Serial.println("Set successful");
   // } else {
    //  Serial.println("Set failed");
      // Serial.println(fbdo.errorReason());
   // }
  }
}
