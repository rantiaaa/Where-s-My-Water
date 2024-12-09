#define BLYNK_TEMPLATE_ID "TMPL66RkUdqmF"                    // Blynk Template ID
#define BLYNK_TEMPLATE_NAME "Where Is My Water"              // Blynk Template Name
#define BLYNK_AUTH_TOKEN "bslM8vPOcqu33TKggLIcs28WbyaNu7FA"  // Blynk Authentication Token

#include <BlynkSimpleEsp32.h>  // Library Blynk
#include <WiFiClient.h>        // Library WiFi Client
#include <WiFi.h>              // Library WiFi
#include <time.h>              // Library untuk waktu NTP

// Pin Virtual Blynk
#define PUMP_BUTTON V0      // Switch pompa manual
#define TRESHOLD_SLIDER V1  // Slider threshold kelembapan
#define MOISTURE_LABEL V2   // Label kelembapan tanah
#define LED_INDICATOR V3    // LED indikator pompa
#define INTERVAL_SLIDER V4  // Slider interval
#define SCHEDULE_INPUT V5   // Input waktu (HH:MM)
#define MODE_SELECTOR V6    // Mode penyiraman

// WiFi Credential
char ssid[] = "find your";
char pass[] = "treasure";

// Input Pin Pada ESP32
const int RELAY_PIN = 32;   // Pin relay untuk pompa
const int SENSOR_PIN = 33;  // Pin sensor kelembapan

// Variabel untuk Waktu NTP
const char *ntpServer = "pool.ntp.org";  // Server NTP
const long gmtOffset_sec = 25200;        // GMT+7
const int daylightOffset_sec = 0;        // Offset daylight saving time
struct tm timeinfo;                      // Variabel waktu

TaskHandle_t xHandleInterval;

// Variabel Kontrol Pompa
bool isPumpOn = false;
bool isManuallyOn = false;
bool isAutomaticallyOn = false;
bool isScheduledOn = false;
bool isIntervalOn = false;

// Variabel Mode
int currentMode = 1;  // Default: automatic with threshold
int thresholdValue = 30;
int sensorPercentage = 0;

// Variabel Input Schedule dan Interval
int scheduleHour = 0;
int scheduleMinute = 0;
int intervalMinutes = 0;

// Timer untuk pompa
BlynkTimer pumpTimer;

// Fungsi untuk mematikan pompa
void stopPump() {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Pompa mati (15 detik selesai)");
  isScheduledOn = false;
  isIntervalOn = false;
}

// Input dari Blynk dalam bentuk mode selection
BLYNK_WRITE(MODE_SELECTOR) {
  currentMode = param.asInt();
  Serial.print("Mode changed to ");
  Serial.println(currentMode);
}

// Input dari Blynk dalam bentuk swith untuk mode manual
BLYNK_WRITE(PUMP_BUTTON) {
  isManuallyOn = param.asInt();
  Serial.print("Manual Pump: ");
  Serial.println(isManuallyOn ? "ON" : "OFF");
}

// Inpur dari Blynk dalam bentuk slider untuk mengatur nilai threshold
BLYNK_WRITE(TRESHOLD_SLIDER) {
  thresholdValue = param.asInt();
  Serial.print("New threshold: ");
  Serial.println(thresholdValue);
}

// Input dari Blynk berupa number input untuk mengatur nilai interval
BLYNK_WRITE(INTERVAL_SLIDER) {
  intervalMinutes = param.asInt();
  //lastIntervalTime = millis();
  Serial.print("New interval: ");
  Serial.print(intervalMinutes);
  Serial.println(" minutes");
}

// Input dari Blynk berupa jam dan menit untuk mengatur jadwal penyiraman
BLYNK_WRITE(SCHEDULE_INPUT) {
  TimeInputParam t(param);
  scheduleHour = t.getStartHour();
  scheduleMinute = t.getStartMinute();
  Serial.print("Schedule set to: ");
  Serial.print(scheduleHour);
  Serial.print(":");
  Serial.println(scheduleMinute);
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  digitalWrite(RELAY_PIN, HIGH);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Konfigurasi Waktu NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Mengambil waktu NTP...");

  // Loop sampai mendapatkan waktu
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Gagal mendapatkan waktu NTP, coba lagi...");
    delay(1000);
  }
  Serial.println("Waktu NTP berhasil didapatkan!");

  // Multitask untuk mengatur pembacaan sensor dan mengontrol pompa
  xTaskCreatePinnedToCore(vTaskSensor, "TaskSensor", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(vTaskPump, "TaskPump", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(vTaskInterval, "TaskInterval", 10000, NULL, 1, &xHandleInterval, 0);
}

// Task untuk pengaturan interval
void vTaskInterval(void *pvParam) {
  while (1) {
    digitalWrite(RELAY_PIN, LOW);
    Blynk.virtualWrite(LED_INDICATOR, 1);
    Serial.println("Pump ON");
    pumpTimer.setTimeout(15000L, stopPump);
    vTaskDelay(pdMS_TO_TICKS(intervalMinutes * 60000));
  }
}

// Task untuk mengatur sistem pembacaan sensor
void vTaskSensor(void *pvParam) {
  while (1) {
    if (!isManuallyOn) {
      int sensorReading = analogRead(SENSOR_PIN);
      sensorPercentage = map(sensorReading, 0, 4095, 0, 100);
      Blynk.virtualWrite(MOISTURE_LABEL, sensorPercentage);
      Serial.print("Moisture: ");
      Serial.println(sensorPercentage);

      if (currentMode == 1 && sensorPercentage < thresholdValue) {
        isAutomaticallyOn = true;
      } else {
        isAutomaticallyOn = false;
      }
    }
    // Sensor akan membaca nilai kelembaban tanah setiap 3 detik
    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

// Task untuk mengatur nyala atau matinya pompa
void vTaskPump(void *pvParam) {
  while (1) {
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      vTaskDelay(pdMS_TO_TICKS(1000));
      continue;
    }

    int currentHour = timeinfo.tm_hour;
    int currentMinute = timeinfo.tm_min;

    // Schedule Mode
    if (currentMode == 2 && currentHour == scheduleHour && currentMinute == scheduleMinute && !isPumpOn) {
      digitalWrite(RELAY_PIN, LOW);
      Blynk.virtualWrite(LED_INDICATOR, 1);
      Serial.println("Pump ON");
      isScheduledOn = true;
      isPumpOn = true;
      pumpTimer.setTimeout(15000L, stopPump);
    } else if (currentMode == 2 && currentHour != scheduleHour && currentMinute != scheduleMinute) {
      digitalWrite(RELAY_PIN, HIGH);
      Blynk.virtualWrite(LED_INDICATOR, 0);
      Serial.println("Pump OFF");
      isPumpOn = false;
    }

    // Interval Mode
    if (currentMode == 3) {
      vTaskResume(xHandleInterval);

    } else if (currentMode != 3) {
      vTaskSuspend(xHandleInterval);
    }

    // Kontrol Pompa
    if ((currentMode == 0 && isManuallyOn) || (currentMode == 1 && isAutomaticallyOn) || (currentMode == 2 && isScheduledOn)) {
      digitalWrite(RELAY_PIN, LOW);
      Blynk.virtualWrite(LED_INDICATOR, 1);
      Serial.println("Pump ON");
    } else if (currentMode == 3) {
      // Mode Interval diatur dengan task interval yang sudah ada
    } else {
      digitalWrite(RELAY_PIN, HIGH);
      Blynk.virtualWrite(LED_INDICATOR, 0);
      Serial.println("Pump OFF");
    }
    // Update setiap 1 detik
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  Blynk.run();
  pumpTimer.run();
}