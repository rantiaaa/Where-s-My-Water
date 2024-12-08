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

// Pin Hardware
const int RELAY_PIN = 32;   // Pin relay
const int SENSOR_PIN = 33;  // Pin sensor kelembapan

// Waktu NTP
const char *ntpServer = "pool.ntp.org";  // Server NTP
const long gmtOffset_sec = 25200;        // GMT+7
const int daylightOffset_sec = 0;        // Offset daylight saving time
struct tm timeinfo;                      // Variabel waktu

// Variabel Kontrol Pompa
bool isPumpOn = false;
bool isManuallyOn = false;
bool isAutomaticallyOn = false;
bool isScheduledOn = false;
bool isIntervalOn = false;

// Variabel Mode
int currentMode = 1;  // Default: automatic threshold
int thresholdValue = 30;
int sensorPercentage = 0;

int scheduleHour = 0, scheduleMinute = 0;
int intervalMinutes = 0;
unsigned long lastIntervalTime = 0;

BlynkTimer pumpTimer;  // Timer untuk pompa

// Fungsi untuk mematikan pompa
void stopPump() {
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Pompa mati (15 detik selesai)");
  isScheduledOn = false;
  isIntervalOn = false;
}

// Input dari Blynk
BLYNK_WRITE(MODE_SELECTOR) {
  currentMode = param.asInt();
  Serial.print("Mode changed to ");
  Serial.println(currentMode);
}

BLYNK_WRITE(PUMP_BUTTON) {
  isManuallyOn = param.asInt();
  Serial.print("Manual Pump: ");
  Serial.println(isManuallyOn ? "ON" : "OFF");
}

BLYNK_WRITE(TRESHOLD_SLIDER) {
  thresholdValue = param.asInt();
  Serial.print("New threshold: ");
  Serial.println(thresholdValue);
}

BLYNK_WRITE(INTERVAL_SLIDER) {
  intervalMinutes = param.asInt();
  lastIntervalTime = millis();
  Serial.print("New interval: ");
  Serial.print(intervalMinutes);
  Serial.println(" minutes");
}

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

  while (!getLocalTime(&timeinfo)) {  // Loop sampai mendapatkan waktu
    Serial.println("Gagal mendapatkan waktu NTP, coba lagi...");
    delay(1000);
  }
  Serial.println("Waktu NTP berhasil didapatkan!");

  // Task multitasking
  xTaskCreatePinnedToCore(vTaskSensor, "TaskSensor", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(vTaskPump, "TaskPump", 10000, NULL, 1, NULL, 1);
}

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
    vTaskDelay(pdMS_TO_TICKS(3000));  // Delay 3 detik
  }
}

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
    if (currentMode == 3 && millis() - lastIntervalTime >= intervalMinutes * 60000) {
      isIntervalOn = true;
      lastIntervalTime = millis();
      pumpTimer.setTimeout(15000L, stopPump);
    }

    // Kontrol Pompa
    if ((currentMode == 0 && isManuallyOn) || (currentMode == 1 && isAutomaticallyOn) || (currentMode == 2 && isScheduledOn) || (currentMode == 3 && isIntervalOn)) {
      digitalWrite(RELAY_PIN, LOW);
      Blynk.virtualWrite(LED_INDICATOR, 1);
      Serial.println("Pump ON");
    } else {
      digitalWrite(RELAY_PIN, HIGH);
      Blynk.virtualWrite(LED_INDICATOR, 0);
      Serial.println("Pump OFF");
    }

    vTaskDelay(pdMS_TO_TICKS(1000));  // Update setiap 1s
  }
}

void loop() {
  Blynk.run();
  pumpTimer.run();
}