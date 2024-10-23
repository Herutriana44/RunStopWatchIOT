// #include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Inisialisasi LCD
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// Variabel stopwatch
unsigned long mulai, selesai, dataStopWatch;
int i = 0;
int fPaus = 0;
long lastButton = 0; 
long delayAntiBouncing = 50; 
long dataPaus = 0;
const int InfraredStar = 3;
const int InfraredStop = 4;
int buzzer = 27;
int star;
int flag = 0;
int resetButton = 25;
void (*ku_reset)(void) = 0;

#define WIFI_SSID "HERU TRIANA X250"
#define WIFI_PASSWORD "heru1234"

#define INTERNAL_LED 2

// Konfigurasi Firebase
#define REFERENCE_URL "https://runningstopwatch-default-rtdb.asia-southeast1.firebasedatabase.app/"
// #define FIREBASE_AUTH "AIzaSyBf4pnbvP_XMgu7edV6zw-cyEzRqC5l1_I"
#define FIREBASE_AUTH "35go3AJ3TO1Ke6oMvs0fm6NmkVRpE6uqEFjGZmPE"
// Firebase firebase(REFERENCE_URL);

struct TimerData {
  int start;
  int finish;
};

void setup() {
    resetFirebaseValues();
    // Setup pin
    pinMode(resetButton, INPUT_PULLUP); 
    pinMode(buzzer, OUTPUT);

    // digitalWrite(buzzer, HIGH);  // Aktifkan buzzer
    // delay(3000);  // Tunda untuk memastikan pengguna melihat pesan
    // digitalWrite(buzzer, LOW);  // Matikan buzzer

    // Inisialisasi LCD
    lcd.init();                 
    lcd.backlight();
    
    
    // Inisialisasi Serial dan Bluetooth
    // Serial.begin(9600);
    // SerialBT.begin("ESP32_Stopwatch"); // Inisialisasi Bluetooth
    // Serial.println("Bluetooth Started!");
    // digitalWrite(buzzer, HIGH);
    
    
    // Koneksi ke Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Menghubungkan ke Wi-Fi...");

        lcd.setCursor(0, 0); 
    lcd.print("Menghubungkan");
    lcd.setCursor(0, 1); 
    lcd.print("ke Wi-Fi...");
    }

    lcd.setCursor(0, 0); 
    lcd.print("    *ESP32    ");
    lcd.setCursor(0, 1); 
    lcd.print("   *StopWatch   ");
    Serial.println("Terhubung ke Wi-Fi");
}

void loop() {
    // Ambil data dari Firebase
    TimerData timerData = getDataFromFirebase();
    int start = timerData.start;
    int finish = timerData.finish;
    
    // Serial.print("start : ");
    // Serial.println(start);
    // Serial.print("finish : ");
    // Serial.println(finish);

    // Cek apakah ada perintah "start" dari Firebase
    if (finish == 1) {
          lcd.setCursor(0, 0);
          lcd.print("Stop Timer      ");
          digitalWrite(buzzer,HIGH);
          delay(3000);
           digitalWrite(buzzer,LOW);
    if ((millis() - lastButton) > delayAntiBouncing){
      if (i==0){

          lcd.clear();
          lcd.setCursor(0, 0);
           lcd.print("Stop Timer      ");
          mulai = millis();
          fPaus = 0;
        }
     
       i =!i;
      }
      lastButton = millis();

  }

    // Cek apakah ada perintah "finish" dari Firebase
    else if (start == 1) {
          digitalWrite(buzzer,HIGH);
          delay(300);
           digitalWrite(buzzer,LOW);
        lcd.setCursor(0, 0);
        lcd.print("Start Timer     ");
if ((millis() - lastButton) > delayAntiBouncing){
  if (i==1){
    lcd.clear();
        lcd.setCursor(0, 0);
         lcd.print("Start Timer     ");
        dataPaus = dataStopWatch;
        fPaus = 1;
        }
       i =!i;
      }
      lastButton = millis();

  }

    // Menjalankan Stopwatch
    if (i == 1) {
        selesai = millis(); 
        float jam, menit, detik, miliDetik;
        unsigned long over;

        // Perhitungan waktu stopwatch
        dataStopWatch = selesai - mulai;
        dataStopWatch = dataPaus + dataStopWatch;

        jam = int(dataStopWatch / 3600000);
        over = dataStopWatch % 3600000;
        menit = int(over / 60000);
        over = over % 60000;
        detik = int(over / 1000);
        miliDetik = over % 1000;

        lcd.setCursor(0, 1);
        lcd.print(jam, 0); 
        lcd.print(":"); 
        lcd.print(menit, 0);
        lcd.print(":");
        lcd.print(detik, 0);
        lcd.print(".");

        Serial.print(jam, 0); 
        Serial.print(":"); 
        Serial.print(menit, 0);
        Serial.print(":");
        Serial.print(detik, 0);
        Serial.print(" ");

        delay(1);

        if (jam < 10) {
            lcd.print(miliDetik, 0);
            lcd.print("   ");
        }
    }

    if(Serial.available() > 0){
      star = Serial.read();
      flag=0;
    }
    if (star == '1') {
      ku_reset();
      if(flag == 0){
      flag=1;
  }
}

    if (digitalRead(resetButton) == LOW) {
      digitalWrite(buzzer, LOW);  // Aktifkan buzzer
    delay(300);  // Tunda untuk memastikan pengguna melihat pesan
    digitalWrite(buzzer, HIGH); 
        delay(200);  // Debounce untuk tombol
        if (digitalRead(resetButton) == LOW) {
            resetFirebaseValues();  // Set nilai start dan finish menjadi 0 di Firebase
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("System Restart ");
            delay(2000);  // Tampilkan pesan reset di LCD
            ESP.restart();  // Melakukan restart pada ESP32
        }
    }
}

void resetFirebaseValues() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Mengatur "start" menjadi 0 di Firebase
        String url = String(REFERENCE_URL) + "start.json?auth=" + FIREBASE_AUTH;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.PUT("0");
        if (httpResponseCode > 0) {
            Serial.println("Start reset to 0");
        } else {
            Serial.println("Error resetting 'start'");
        }
        http.end();

        // Mengatur "finish" menjadi 0 di Firebase
        url = String(REFERENCE_URL) + "finish.json?auth=" + FIREBASE_AUTH;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        httpResponseCode = http.PUT("0");
        if (httpResponseCode > 0) {
            Serial.println("Finish reset to 0");
        } else {
            Serial.println("Error resetting 'finish'");
        }
        http.end();
    } else {
        Serial.println("WiFi is not connected");
    }
}

// Gantikan fungsi Firebase dengan HTTPClient untuk REST API
TimerData getDataFromFirebase() {
    TimerData data;  // Membuat struct untuk menyimpan nilai

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // Ambil data "start" dari Firebase
        String url = String(REFERENCE_URL) + "start.json?auth=" + FIREBASE_AUTH;
        http.begin(url);
        int httpCode = http.GET();
        if (httpCode > 0) {
            String payload = http.getString();
            data.start = payload.toInt();
        } else {
            Serial.println("Error getting 'start' data from Firebase");
        }
        http.end();

        // Ambil data "finish" dari Firebase
        url = String(REFERENCE_URL) + "finish.json?auth=" + FIREBASE_AUTH;
        http.begin(url);
        httpCode = http.GET();
        if (httpCode > 0) {
            String payload = http.getString();
            data.finish = payload.toInt();
        } else {
            Serial.println("Error getting 'finish' data from Firebase");
        }
        http.end();
    } else {
        Serial.println("WiFi is not connected");
    }
    return data;  // Mengembalikan struct dengan start dan finish
}
