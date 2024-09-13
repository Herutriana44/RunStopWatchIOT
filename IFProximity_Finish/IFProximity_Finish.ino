#include <WiFi.h>
#include <Firebase_ESP32.h>

// Konfigurasi WiFi
#define WIFI_SSID "HERU TRIANA X250"
#define WIFI_PASSWORD "heru1234"

// Konfigurasi Firebase
#define FIREBASE_HOST "https://runningstopwatch-default-rtdb.asia-southeast1.firebasedatabase.app/"
// #define FIREBASE_AUTH "token_autentikasi_anda"
#define FIREBASE_DB_SECRET "35go3AJ3TO1Ke6oMvs0fm6NmkVRpE6uqEFjGZmPE"

// Pin untuk sensor inframerah
#define TRIG_PIN 2
#define ECHO_PIN 4

// Buat objek Firebase
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, FIREBASE_DB_SECRET);
  Firebase.begin(FIREBASE_HOST, FIREBASE_DB_SECRET);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Baca nilai sensor inframerah (Anda perlu menyesuaikan fungsi ini sesuai dengan sensor Anda)
  // Misalnya, jika nilai analog lebih besar dari nilai ambang batas, berarti ada objek
  if (// kondisi jika ada objek terdeteksi) {
    Serial.println("Objek terdeteksi!");
    
    // Kirim data ke Firebase
    Firebase.setJSON(firebaseData, "/data", "{\"start\":true, \"finish\":true}");
    if (firebaseData.error) {
      Serial.println("Set failed");
      Serial.println(firebaseData.errorReason());
    } else {
      Serial.println("Set successful");
    }

    // Delay 5 detik
    delay(5000);
    
    Firebase.setJSON(firebaseData, "/data", "{\"start\":false, \"finish\":false}");
    if (firebaseData.error) {
      Serial.println("Set failed");
      Serial.println(firebaseData.errorReason());
    } else {
      Serial.println("Set successful");
    }
  }
}