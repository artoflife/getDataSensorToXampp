#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

const char WIFI_SSID[] = "GOCAN";
const char WIFI_PASSWORD[] = "gofic1234";
String HOST_NAME = "http://192.168.100.230"; // GANTI dengan IP PC/XAMPP Anda
String PATH_NAME = "/ESP/insert_temp.php";// ganti sesuai IP PC

// DHT config
#define DHTPIN 4        // ganti sesuai pin data DHT yang kamu pakai (contoh GPIO4)
#define DHTTYPE DHT22   // ganti ke DHT11 jika pakai DHT11
DHT dht(DHTPIN, DHTTYPE);

// interval kirim (5000 ms = 5 detik)
const unsigned long SEND_INTERVAL = 5000UL;
unsigned long lastSend = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start > 30000) { // timeout 30s
      Serial.println("\nTimeout koneksi WiFi. Cek SSID/password atau jaringan.");
      // jangan gunakan 'continue' di sini, gunakan return agar keluar dari setup()
      return;
    }
  }
  Serial.println("");
  Serial.print("Terkoneksi dengan Alamat IP : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // kirim setiap 5 detik
  if (millis() - lastSend >= SEND_INTERVAL) {
    lastSend = millis();

    // baca sensor
    float hum = dht.readHumidity();
    float temp = dht.readTemperature(); // Celsius

    if (isnan(hum) || isnan(temp)) {
      Serial.println("Gagal baca DHT sensor!");
      // 'continue' di sini valid karena berada di dalam loop()
      return; // atau gunakan continue; jika ingin tetap berada dalam loop() dan lanjut iterasi
    }

    // buat query string dengan 2 desimal
    String queryString = "?temperature=" + String(temp, 2) + "&humidity=" + String(hum, 2);
    String url = HOST_NAME + PATH_NAME + queryString;

    Serial.println("Request: " + url);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(url); // HTTP
      http.setTimeout(10000);
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println("Response: " + payload);
        } else {
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.println("WiFi tidak terhubung. Skip kirim.");
    }
  }

  // sedikit delay agar loop tidak sibuk
  delay(10);
}