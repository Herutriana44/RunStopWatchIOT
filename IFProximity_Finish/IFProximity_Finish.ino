#include <WiFi.h>
#include <ESP32Firebase.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Konfigurasi WiFi - Tambahkan beberapa SSID dan Password ke array
const char* ssidList[] = {"KOSAN IBU NUR", "Ruang Aslab", "HERU TRIANA X250"};
const char* passwordList[] = {"sikembar", "labkom2023", "heru1234"};
const int wifiCount = sizeof(ssidList) / sizeof(ssidList[0]);  // Total jumlah jaringan Wi-Fi

#define INTERNAL_LED 2

// Konfigurasi Firebase
#define REFERENCE_URL ""
#define FIREBASE_AUTH ""
#define FIREBASE_DB_SECRET ""
Firebase firebase(REFERENCE_URL);

// Pin untuk sensor inframerah
const int buttonPin = 26;
int buttonState = 0;

void setup() {
  Serial.begin(9600);

  // Coba sambungkan ke Wi-Fi dari daftar
  bool connected = false;
  for (int i = 0; i < wifiCount; i++) {
    Serial.print("Connecting to Wi-Fi: ");
    Serial.println(ssidList[i]);
    WiFi.begin(ssidList[i], passwordList[i]);
    
    // Tunggu hingga terhubung atau timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {  // 20 * 500ms = 10 detik untuk setiap Wi-Fi
      Serial.print(".");
      delay(500);
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      Serial.println();
      Serial.print("Connected to Wi-Fi with IP: ");
      Serial.println(WiFi.localIP());
      break;  // Keluar dari loop jika terhubung
    } else {
      Serial.println("\nFailed to connect. Trying next network...");
    }
  }

  if (!connected) {
    Serial.println("Failed to connect to any Wi-Fi network.");
    while (true);  // Berhenti jika tidak dapat terhubung ke jaringan mana pun
  }

  pinMode(buttonPin, INPUT);
  Serial.println("Program dimulai");
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    Serial.println("Objek terdeteksi!");
    
    // Kirim data ke Firebase
    if (firebase.setInt("finish", 1)) {
       digitalWrite(INTERNAL_LED,HIGH);
      Serial.println("Set successful");
    } else {
      Serial.println("Set failed");
    }

    // Delay 1 detik
    delay(1000);
  } else {
    Serial.println("Objek tidak terdeteksi!");
    digitalWrite(INTERNAL_LED,LOW);
  }
}
