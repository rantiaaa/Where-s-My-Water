#define BLYNK_TEMPLATE_ID "TMPL6aWJWiBXI"
#define BLYNK_TEMPLATE_NAME "Where Is My Water"
#define BLYNK_AUTH_TOKEN "jfKtNjEaqshOMLlgrnTdU7axu1SNl064"

#include <BlynkSimpleEsp32.h> //Menambahkan library blynk
#include <WiFiClient.h> // Menambahkan library wifi
#include <WiFi.h> // Menambahkan library wifi (juga)

#define PUMP_BUTTON V0
#define TRESHOLD_SLIDER V1
#define MOISTURE_LABEL V2
#define LED_INDICATOR V3

char ssid[] = "find your"; // Nama Wifi
char pass[] = "treasure"; // Password Wifi

const int RELAY_PIN = 32;
const int SENSOR_PIN = 33;

bool isManuallyOn = false;
bool isAutomaticallyOn = false;
int thresholdValue = 0;
int thresholdPercentage = 0;
int sensorReading = 0;
int sensorPercentage = 0;


BLYNK_WRITE(PUMP_BUTTON){
  if(param.asInt() == 1){
    isManuallyOn = true;
    Serial.println("Manually on");
  }else if(param.asInt() == 0){
    isManuallyOn = false;
        Serial.println("Manually off");

  }
}

BLYNK_WRITE(TRESHOLD_SLIDER){
  thresholdValue = param.asInt();
  Serial.print("New threshold: ");
  Serial.println(thresholdValue);
}

void setup(){
  
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass); 

  xTaskCreatePinnedToCore(vTaskSensor, "TaskSensor", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(vTaskPump, "TaskPump", 10000, NULL, 1, NULL, 1);

}


void vTaskSensor(void *pvParam){
  while(1){
    if(!isManuallyOn){
      sensorReading = analogRead(SENSOR_PIN);
      sensorPercentage = map(sensorReading, 0, 4095, 0, 100);
      Serial.println(sensorPercentage);
      Blynk.virtualWrite(MOISTURE_LABEL, sensorPercentage);
      if(sensorPercentage < thresholdValue){
        isAutomaticallyOn = true;
      }else{
        isAutomaticallyOn = false;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void vTaskPump(void *pvParam){
  while(1){
    if(isManuallyOn || isAutomaticallyOn){
      digitalWrite(RELAY_PIN, LOW);
      Blynk.virtualWrite(LED_INDICATOR, 1);
          Serial.println("Pump is on!");

    }else{
      digitalWrite(RELAY_PIN, HIGH);
      Blynk.virtualWrite(LED_INDICATOR, 0);
          Serial.println("Pump is off!");
 
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void loop(){
  Blynk.run(); 
}