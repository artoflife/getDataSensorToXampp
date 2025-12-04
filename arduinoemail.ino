#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include "DHT.h"

#define DHTPIN 4        // Pin sensor DHT
#define DHTTYPE DHT22   // atau DHT11, tergantung sensor kamu

#define WIFI_SSID "GOCAN"
#define WIFI_PASSWORD "gofic1234"

// Email setting
#define SENDER_EMAIL "sukamangga31@gmail.com"
#define SENDER_PASSWORD "gkex vgsq rheq olhl" // Gunakan App Password Gmail
#define RECIPIENT_EMAIL "berlianamahadewi2103@gmail.com"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 587

// Batas suhu normal
#define BATAS_SUHU 32.0

DHT dht(DHTPIN, DHTTYPE);
SMTPSession smtp;

void setup() {
  Serial.begin(115200);
  dht.begin();

  Serial.println("Menghubungkan ke Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWi-Fi terhubung!");
  Serial.println(WiFi.localIP());
}

void loop() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca sensor DHT!");
    delay(2000);
    return;
  }

  Serial.print("Suhu: "); Serial.print(suhu); Serial.print(" °C | ");
  Serial.print("Kelembapan: "); Serial.print(kelembapan); Serial.println(" %");

  // RULE: Jika suhu > batas, kirim email
  if (suhu > BATAS_SUHU) {
    String subject = "⚠️ Peringatan Suhu Greenhouse Tinggi!";
    String msg = "Suhu saat ini: " + String(suhu, 1) + " °C\n"
                 "Kelembapan: " + String(kelembapan, 1) + " %\n\n"
                 "Segera ambil tindakan pendinginan.";

    gmail_send(subject, msg);
  } else {
    Serial.println("Suhu masih dalam batas normal.\n");
  }

  delay(5000); // cek setiap 1 menit
}

void gmail_send(String subject, String textMsg) {
  MailClient.networkReconnect(true);
  smtp.debug(0); // ubah ke 1 kalau mau lihat log detail
  smtp.callback(smtpCallback);

  Session_Config config;
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = SENDER_EMAIL;
  config.login.password = SENDER_PASSWORD;
  config.login.user_domain = F("127.0.0.1");
  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 7 * 3600; // GMT+7 Indonesia
  config.time.day_light_offset = 0;

  SMTP_Message message;
  message.sender.name = F("Greenhouse Monitor ESP32");
  message.sender.email = SENDER_EMAIL;
  message.subject = subject;
  message.addRecipient(F("Operator Greenhouse"), RECIPIENT_EMAIL);
  message.text.content = textMsg;
  message.text.transfer_encoding = "base64";
  message.text.charSet = F("utf-8");
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;

  if (!smtp.connect(&config)) {
    Serial.printf("Gagal koneksi SMTP: %s\n", smtp.errorReason().c_str());
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.printf("Gagal kirim email: %s\n", smtp.errorReason().c_str());
  } else {
    Serial.println("✅ Email notifikasi berhasil dikirim!");
  }

  smtp.sendingResult.clear();
}

void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());
  if (status.success()) {
    Serial.printf("Email sukses terkirim (%d berhasil, %d gagal)\n",
                  status.completedCount(), status.failedCount());
  }
}