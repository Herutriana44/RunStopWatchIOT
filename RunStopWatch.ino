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

// Konfigurasi WiFi - Tambahkan beberapa SSID dan Password ke array
const char* ssidList[] = {"LAPTOP-U1N0FTTS 8991", "HERU TRIANA X250"};
const char* passwordList[] = {"729d7)G6", "heru1234"};
const int wifiCount = sizeof(ssidList) / sizeof(ssidList[0]);

#define INTERNAL_LED 2

// Konfigurasi Firebase
#define REFERENCE_URL "https://runningstopwatch-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "35go3AJ3TO1Ke6oMvs0fm6NmkVRpE6uqEFjGZmPE"

struct TimerData {
  int start;
  int finish;
  int reset;
};

void setup() {
    Serial.begin(9600);
    resetFirebaseValues();
    
    // Setup pin
    pinMode(resetButton, INPUT_PULLUP); 
    pinMode(buzzer, OUTPUT);
    
    lcd.init();                 
    lcd.backlight();
    
    // Koneksi ke Wi-Fi dari daftar jaringan yang tersedia
    bool connected = false;
    for (int i = 0; i < wifiCount; i++) {
        Serial.print("Connecting to Wi-Fi: ");
        Serial.println(ssidList[i]);
        WiFi.begin(ssidList[i], passwordList[i]);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            connected = true;
            Serial.println();
            Serial.print("Connected to Wi-Fi with IP: ");
            Serial.println(WiFi.localIP());
            break;
        } else {
            Serial.println("\nFailed to connect. Trying next network...");
        }
    }

    if (!connected) {
        Serial.println("Failed to connect to any Wi-Fi network.");
        lcd.setCursor(0, 0);
        lcd.print("No Wi-Fi found");
        while (true);
    }

    lcd.setCursor(0, 0); 
    lcd.print("    *ESP32    ");
    Serial.println("    *ESP32    ");
    lcd.setCursor(0, 1); 
    lcd.print("   *StopWatch   ");
    Serial.println("   *StopWatch   ");
}

void loop() {
    TimerData timerData = getDataFromFirebase();
    int start = timerData.start;
    int finish = timerData.finish;
    int reset = timerData.reset;

    if (finish == 1) {
          lcd.setCursor(0, 0);
          lcd.print("Stop Timer      ");
          Serial.println("Stop Timer      ");
          digitalWrite(buzzer,HIGH);
          delay(3000);
          digitalWrite(buzzer,LOW);
          if ((millis() - lastButton) > delayAntiBouncing) {
              if (i == 0) {
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Stop Timer      ");
                  Serial.println("Stop Timer      ");
                  mulai = millis();
                  fPaus = 0;
              }
              i = !i;
          }
          lastButton = millis();
    }

    else if (start == 1) {
          digitalWrite(buzzer, HIGH);
          delay(300);
          digitalWrite(buzzer, LOW);
          lcd.setCursor(0, 0);
          lcd.print("Start Timer     ");
          Serial.println("Start Timer      ");
          if ((millis() - lastButton) > delayAntiBouncing) {
              if (i == 1) {
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Start Timer     ");
                  Serial.println("Start Timer      ");
                  dataPaus = dataStopWatch;
                  fPaus = 1;
              }
              i = !i;
          }
          lastButton = millis();
    }

    if (i == 1) {
        selesai = millis(); 
        float jam, menit, detik, miliDetik;
        unsigned long over;

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
        Serial.println();

        Serial.print(jam); 
        Serial.print(":"); 
        Serial.print(menit);
        Serial.print(":");
        Serial.print(detik);
        Serial.print(".");
        Serial.println();

        delay(1);

        if (jam < 10) {
            lcd.print(miliDetik, 0);
            lcd.print("   ");
        }
    }

    if (digitalRead(resetButton) == LOW || reset == 1) {
      digitalWrite(buzzer, LOW);
      delay(300);
      digitalWrite(buzzer, HIGH);
      delay(200);  
      if (digitalRead(resetButton) == LOW) {
          resetFirebaseValues();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("System Restart ");
          Serial.println("System Restart ");
          delay(2000);
          ESP.restart();
      }
    }
}

void resetFirebaseValues() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        String url = String(REFERENCE_URL) + "start.json?auth=" + FIREBASE_AUTH;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.PUT("0");
        http.end();

        url = String(REFERENCE_URL) + "finish.json?auth=" + FIREBASE_AUTH;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        httpResponseCode = http.PUT("0");
        http.end();
    } else {
        Serial.println("WiFi is not connected");
    }
}

TimerData getDataFromFirebase() {
    TimerData data;

    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        String url = String(REFERENCE_URL) + "start.json?auth=" + FIREBASE_AUTH;
        http.begin(url);
        int httpCode = http.GET();
        if (httpCode > 0) {
            data.start = http.getString().toInt();
        }
        http.end();

        url = String(REFERENCE_URL) + "finish.json?auth=" + FIREBASE_AUTH;
        http.begin(url);
        httpCode = http.GET();
        if (httpCode > 0) {
            data.finish = http.getString().toInt();
        }
        http.end();
    } else {
        Serial.println("WiFi is not connected");
    }
    return data;
}
