#include <WiFi.h>
#include <FirebaseESP32.h>

// Konfigurasi WiFi
#define WIFI_SSID "HERU TRIANA X250"
#define WIFI_PASSWORD "heru1234"

#define INTERNAL_LED 2

// Konfigurasi Firebase
#define FIREBASE_HOST "https://runningstopwatch-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH ""
#define FIREBASE_DB_SECRET "35go3AJ3TO1Ke6oMvs0fm6NmkVRpE6uqEFjGZmPE"

// Pin untuk sensor inframerah
const int buttonPin = 26;
int buttonState = 0;

// Buat objek Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

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

  // // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, FIREBASE_DB_SECRET);
  config.database_url = FIREBASE_HOST;
  config.api_key = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

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
    // Firebase.setJSON(fbdo, "/data", "{\"start\":true, \"finish\":false}");
    if (Firebase.setInt(fbdo, "start", 1)) {
      Serial.println("Set successful");
    } else {
      Serial.println("Set failed");
      Serial.println(fbdo.errorReason());
    }

    // // Delay 3 detik
    delay(1000);
  } else {
    Serial.println("Objek tidak terdeteksi!");
    digitalWrite(INTERNAL_LED,LOW);
    if (Firebase.setInt(fbdo, "start", 0)) {
      Serial.println("Set successful");
    } else {
      Serial.println("Set failed");
      Serial.println(fbdo.errorReason());
    }
  }
}