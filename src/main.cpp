#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include "time.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>
#include <ESP32Servo.h>

#define DHTPIN 4
#define DHTTYPE 22

#define BUZZER_PIN 12
#define LDRPIN 34

void display_adjust() ;
void color(int colour);
void play_music();
void play_temp();
int set_time();
int set_time_offset();
int set_alarm_1();
int set_alarm_2();
void remove_alarm();
void update_data();
void update_menu();
void go_to_menu(int select, String menu_items[], int size);
void receiveCallback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT();
void setupWiFi();

// WiFi credentials
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

Servo myServo;      // create servo object
int servoPin = 23;  // any PWM-capable GPIO
int angle = 0;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1

struct tm timeinfo;

int melody[] = {262, 294, 330, 349, 392, 440, 494, 523}; 

static const uint8_t image_data_Saraarray[1024] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0x83, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xfe, 0x79, 0xf3, 0xcf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xfc, 0xfe, 0xef, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xfd, 0xff, 0x1f, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xfb, 0xff, 0xbf, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xfb, 0xff, 0xff, 0xfb, 0xff, 0x9f, 0xf9, 0x80, 0x60, 0x3f, 0x70, 0x1f, 0xc0, 0x7e, 0x7e, 0x7f, 
    0xfb, 0xff, 0xff, 0xfb, 0xff, 0x9f, 0xf1, 0x9f, 0xe7, 0x8f, 0x73, 0xcf, 0x8f, 0x3f, 0x7c, 0xff, 
    0xfb, 0xff, 0xff, 0xfb, 0xff, 0x8f, 0xf1, 0x9f, 0xe7, 0xc7, 0x77, 0xef, 0x3f, 0x9f, 0x3c, 0xff, 
    0xfb, 0xff, 0xff, 0xfb, 0xff, 0x8f, 0xe1, 0x9f, 0xe7, 0xe7, 0x77, 0xee, 0x7f, 0xcf, 0x99, 0xff, 
    0xfb, 0xff, 0xff, 0xfb, 0xff, 0x8f, 0xe1, 0x9f, 0xe7, 0xf3, 0x77, 0xce, 0x7f, 0xcf, 0xc3, 0xff, 
    0xf9, 0xff, 0xf8, 0x00, 0x7f, 0xa7, 0xc9, 0x9f, 0xe7, 0xf3, 0x70, 0x1e, 0x7f, 0xef, 0xc7, 0xff, 
    0xfd, 0xff, 0xf3, 0xef, 0xbf, 0xb7, 0xc9, 0x81, 0xe7, 0xf3, 0x70, 0x0e, 0xff, 0xe7, 0xe7, 0xff, 
    0xfc, 0xff, 0xf7, 0xef, 0xdf, 0xb3, 0x99, 0x9f, 0xe7, 0xf3, 0x77, 0xe6, 0x7f, 0xef, 0xc3, 0xff, 
    0xfe, 0xff, 0xf7, 0xef, 0xdf, 0xbb, 0x99, 0x9f, 0xe7, 0xf3, 0x77, 0xe6, 0x7f, 0xef, 0x93, 0xff, 
    0xff, 0x7f, 0xf7, 0xef, 0x9f, 0xb9, 0x39, 0x9f, 0xe7, 0xe7, 0x77, 0xf6, 0x7f, 0xcf, 0x99, 0xff, 
    0xff, 0xbf, 0xf8, 0x00, 0x3f, 0xbc, 0x39, 0x9f, 0xe7, 0xe7, 0x77, 0xe7, 0x3f, 0x9f, 0x3c, 0xff, 
    0xff, 0xdf, 0xfe, 0x3f, 0xff, 0xbc, 0x79, 0x9f, 0xe7, 0x8f, 0x73, 0xc7, 0x8f, 0x3e, 0x7e, 0xff, 
    0xff, 0xef, 0xfe, 0x7f, 0xff, 0xbe, 0x79, 0x80, 0x60, 0x3f, 0x70, 0x0f, 0xc0, 0x7e, 0x7e, 0x7f, 
    0xff, 0xf7, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xf9, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xfc, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// MQTT Broker details
//const char* MQTT_BROKER = "test.mosquitto.org";
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_TEMP = "medibox/temp";
const char* MQTT_TOPIC_HUMI = "medibox/humi";
const char* MQTT_TOPIC_LDR = "medibox/ldr";
const char* MQTT_TOPIC_LDR_SAMPLING = "medibox/ldrsampling";
const char* MQTT_TOPIC_LDR_TOTAL = "medibox/ldrtotal";
const char* MQTT_TOPIC_COMMAND = "medibox/buzzer";
const char* MQTT_CLIENT_ID = "ESP32_1234567_Client";
const char* MQTT_TOPIC_CONTROL = "medibox/control";
const char* MQTT_TOPIC_OFFSET = "medibox/offset";
const char* MQTT_TOPIC_BASETEMP = "medibox/basetemp";

// Objects
WiFiClient espClient;
PubSubClient mqttClient(espClient);

//motor variables
float theta = 0.0;
float moffset = 30.0;
float controlling_factor = 0.75;
float ideal_temp = 30;

// Variables
int single_time = 5;
int total_time = 15;
int max_count = total_time/single_time;
volatile float ldr_avg = 0.0;
volatile float ldr = 0.0;
volatile int count = 0;
volatile bool send_ldr = false;

volatile int time_now[3] = {0,0,0};
volatile int time_update[4] = {0,0,0,0};
volatile int alarm_1[4] = {0,0,0,0};
volatile int alarm_2[4] = {0,0,0,0};
volatile int alarm_s[4] = {0,0,0,0};
int alarm_temp[4] = {0,0,0,0};

char timeStr[10];
char time_alarm[10];

const char * ssid = "Wokwi-GUEST";
const char * password = "";

const char * ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

float offset = 0;

volatile int alarm_active = 0;
int temp_alarm = 0;

int item = 0;
String menu[5] = {"1. Set time", "2. Alarm 1", "3. Alarm 2", "4. Remove alarm", "5. Set time offset"};
int menu_size = sizeof(menu) / sizeof(menu[0]);
int position = 0;

int done = 0;

const int max_values[4] = {23, 59, 59, 7};
const char* labels[4] = {"Hour", "Minute", "Second", "Day"};

float temperature;
float humidity;

// Timer interrupt variables
hw_timer_t * timer0 = NULL;
hw_timer_t * timer1 = NULL;
portMUX_TYPE timerMux1 = portMUX_INITIALIZER_UNLOCKED;

// Timer Interrupt Service Routine (ISR)
void IRAM_ATTR onTimer0() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (temperature < 24 || temperature > 32)  {
    temp_alarm = 1;
  } 
  else if (humidity < 65 || humidity > 80) {
    temp_alarm = 1;
  }
  else {
    temp_alarm = 0;
  }
}

void IRAM_ATTR onTimer1() {
  portENTER_CRITICAL_ISR(&timerMux1);
  ldr += (4095.5 - (float)analogRead(LDRPIN)) / 4095.0;  // 12-bit ADC
  count++;

  if (count >= max_count) {
    ldr_avg = (ldr/max_count);
    count = 0;
    ldr = 0;
    send_ldr = true;  // Modified the global variable, not creating a local one
  }
  portEXIT_CRITICAL_ISR(&timerMux1);
}

hw_timer_t *My_timer = NULL;
void IRAM_ATTR onTimer(){
  if (time_update[0] == 1) {
    time_now[0] = time_update[1];
    time_now[1] = time_update[2];
    time_now[2] = time_update[3];
    time_update[0] = 0;
  }

  time_now[2]++;
  
  if (time_now[2] >= 60) {
    time_now[2] = 0;
    time_now[1]++;
    
    if (time_now[1] >= 60) {
      time_now[1] = 0;
      time_now[0]++; 
      
      if (time_now[0] >= 24) {
        time_now[0] = 0;
      }
    }
  }
  
  sprintf(timeStr, "%02d:%02d:%02d", time_now[0], time_now[1], time_now[2]);

  if (alarm_1[0] == 1) {
    if ((time_now[0] == alarm_1[1]) && (time_now[1] == alarm_1[2])) {
      if (time_now[2] == alarm_1[3]) {
        alarm_active=1;
      }
    }
  }
  
  if (alarm_2[0] == 1) {
    if ((time_now[0] == alarm_2[1]) && (time_now[1] == alarm_2[2])) {
      if (time_now[2] == alarm_2[3]) {
        alarm_active=1;
      }
    }
  }

  if (alarm_s[0] == 1) {
    if ((time_now[0] == alarm_s[1]) && (time_now[1] == alarm_s[2])) {
      if (time_now[2] == alarm_s[3]) {
        alarm_active=1;
      }
    }
  }
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
    // Timeout after 10 seconds
    if(millis() - startAttemptTime > 10000) {
      Serial.println("\nFailed to connect to WiFi. Restarting...");
      ESP.restart();
    }
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("connected");
      
      // Subscribe to command topic
      mqttClient.subscribe(MQTT_TOPIC_COMMAND);
      mqttClient.subscribe(MQTT_TOPIC_LDR_SAMPLING);
      mqttClient.subscribe(MQTT_TOPIC_LDR_TOTAL);
      mqttClient.subscribe(MQTT_TOPIC_OFFSET);
      mqttClient.subscribe(MQTT_TOPIC_BASETEMP);
      mqttClient.subscribe(MQTT_TOPIC_CONTROL);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds");
      delay(5000);
    }
  }
}

// Callback function for received MQTT messages
void receiveCallback(char* topic, byte* payload, unsigned int length) {
  char message[100] = {0};  // fixed buffer
  length = min(length, sizeof(message) - 1);  // prevent overflow

  for (unsigned int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

 if (strcmp(topic, MQTT_TOPIC_LDR_SAMPLING) == 0) {
    single_time = atoi(message);  // Convert message to int and update x
    max_count = total_time/single_time;
    Serial.print("Slider value updated to: ");
    Serial.println(max_count);
  } else if (strcmp(topic, MQTT_TOPIC_LDR_TOTAL) == 0) {
    total_time = atoi(message);  // Convert message to int and update x
    max_count = total_time /single_time;
    Serial.print("Slider value updated to: ");
    Serial.println(max_count);
   } else if (strcmp(topic, MQTT_TOPIC_BASETEMP) == 0) {
    ideal_temp = (float)atof(message);  // Convert message to int and update x
    Serial.print("Slider value updated to: ");
    Serial.println(ideal_temp);
   } else if (strcmp(topic, MQTT_TOPIC_OFFSET) == 0) {
    moffset = atof(message);  // Convert message to int and update x
    Serial.print("Slider value updated to: ");
    Serial.println(moffset);
  } else if (strcmp(topic, MQTT_TOPIC_CONTROL) == 0) {
    controlling_factor = atof(message);  // Convert message to int and update x
    Serial.print("Slider value updated to: ");
    Serial.println(controlling_factor);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(16, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(16, LOW); 
  digitalWrite(BUZZER_PIN, LOW); 

  analogReadResolution(12);

  dht.begin();

  setupWiFi();

  // Setup MQTT
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(receiveCallback);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  else {
    sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    time_now[0] = timeinfo.tm_hour;
    time_now[1] = timeinfo.tm_min;
    time_now[2] = timeinfo.tm_hour;
  }

  My_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(My_timer, &onTimer, true);
  timerAlarmWrite(My_timer, 1000000, true);
  timerAlarmEnable(My_timer);

  // Setup timer interrupt
  // 1 MHz timer, interrupt every 1 second (1,000,000 microseconds)
  timer0 = timerBegin(2, 80, true);
  timerAttachInterrupt(timer0, &onTimer0, true);
  timerAlarmWrite(timer0, 1000000, true);
  timerAlarmEnable(timer0);

  timer1 = timerBegin(1, 80, true);
  timerAttachInterrupt(timer1, &onTimer1, true);
  timerAlarmWrite(timer1, 1000000, true);
  timerAlarmEnable(timer1);

  myServo.setPeriodHertz(50);           // Standard servo = 50 Hz
  myServo.attach(servoPin, 500, 2400);  // Min and max pulse width in µs
  myServo.write(angle);                 // Set initial position

  if (!mqttClient.connected()) {
    reconnectMQTT();
  }

  char totalStr[10];
  char singleStr[10];
  char basetempStr[10];
  char controlStr[10];
  char offsetStr[10];

  dtostrf(total_time, 4, 2, totalStr);
  dtostrf(single_time, 4, 2, singleStr);
  dtostrf(moffset, 4, 2, offsetStr);
  dtostrf(controlling_factor, 4, 2, controlStr);
  dtostrf(ideal_temp, 4, 2, basetempStr);

  mqttClient.publish(MQTT_TOPIC_LDR_TOTAL, totalStr);
  mqttClient.publish(MQTT_TOPIC_LDR_SAMPLING, singleStr);
  mqttClient.publish(MQTT_TOPIC_BASETEMP, basetempStr);
  mqttClient.publish(MQTT_TOPIC_CONTROL, controlStr);
  mqttClient.publish(MQTT_TOPIC_OFFSET, offsetStr);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }

  display.clearDisplay();
  
  display.drawBitmap(0, 0, image_data_Saraarray, 128, 64, 1);
  display.display();
  delay(2000);
  go_to_menu(item, menu, menu_size);
}

void loop() {
// Reconnect MQTT if needed
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  float templocal = temperature;
  float humilocal = humidity;
  
  portENTER_CRITICAL(&timerMux1);
  float ldrlocal = ldr_avg;
  bool should_send_ldr = send_ldr;
  if (send_ldr) {
    send_ldr = false;  // Reset the flag after reading
  }
  //Serial.println(max_count);
  portEXIT_CRITICAL(&timerMux1);

  char tempStr[10];
  char humidStr[10];
  char ldrStr[10];

  // Convert float to string
  dtostrf(templocal, 4, 2, tempStr);
  dtostrf(humilocal, 4, 2, humidStr);
  dtostrf(ldrlocal, 4, 2, ldrStr);

  // Publish temperature
  mqttClient.publish(MQTT_TOPIC_TEMP, tempStr);

  // Publish humidity
  mqttClient.publish(MQTT_TOPIC_HUMI, humidStr);
  
  // Only publish LDR when there's a new sample ready
  if (should_send_ldr) {
    mqttClient.publish(MQTT_TOPIC_LDR, ldrStr);
  }

  //motor angle adjust
  theta = abs(moffset + (180.0 - moffset) * ldrlocal * controlling_factor * log(1.0/(float)max_count) * ((float)templocal / ideal_temp));
  myServo.write((int)theta);

  if (digitalRead(26) == 0) {
    switch(item) {
      case 0:
        set_time();
        break;
        
      case 1:
        done = set_alarm_1();
        break;
        
      case 2:
        done = set_alarm_2();
        break;
        
      case 3:
        remove_alarm();
        break;

      case 4:
        set_time_offset();
        break;
    }
    while(digitalRead(26) == 0) {  
      delay(10);
    }
    go_to_menu(item, menu, menu_size);
  }
  else if (digitalRead(25) == 0) {
    update_data();
    while(digitalRead(26) == 0) {  
      delay(10);
    }
    go_to_menu(item, menu, menu_size);
  }
  else if (alarm_active == 1) {
    while(digitalRead(26) == 0) {  
      delay(10);
    }
    play_music();
  }
  else if (temp_alarm == 1) {
    while(digitalRead(26) == 0) {  
      delay(10);
    }
    play_temp();
  }
  else {
    update_menu();
  }  
}

void play_music() {
  display_adjust();
  display.println("Alarm 1 active");
  display.display();

  unsigned long previousMillis = 0;
  const unsigned long interval = 500;
  bool ledState = LOW;
  int i = 0;

  while(true) {
    unsigned long currentMillis = millis();
    if (i > 8) {
      i=0;
    }
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      i++;
      digitalWrite(16, ledState);
      tone(BUZZER_PIN, melody[i]);
    }

    if (digitalRead(25) == 0) {
      alarm_active = 0;
      digitalWrite(16, LOW);
      noTone(BUZZER_PIN);
      break;
    }

    else if (digitalRead(26) == 0) {
      alarm_active = 0;
      alarm_s[0] = 1;
      alarm_s[2] = alarm_1[1];
      alarm_s[2] = alarm_1[2]+5;
      digitalWrite(16, LOW);
      noTone(BUZZER_PIN);
      break;
    }
  }
}

void play_temp() {
  display_adjust();
  display.println("Temperature and humidity is abnormal!!!!");
  display.display();

  unsigned long previousMillis = 0;
  const unsigned long interval = 500;
  bool ledState = LOW;
  int i = 0;

  while(temp_alarm) {
    unsigned long currentMillis = millis();
    if (i > 4) {
      i=0;
    }
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      i++;
      digitalWrite(16, ledState);
      tone(BUZZER_PIN, melody[i]);
    }
  }
  digitalWrite(16, LOW);
  noTone(BUZZER_PIN);
}

void update_data() {
  while (digitalRead(25) == 0) {
    delay(10);
  }

  while (true) {
    if (alarm_active == 1) {
      while(digitalRead(26) == 0) {  
        delay(10);
      }
      play_music();
    }
    else if (temp_alarm == 1) {
      while(digitalRead(26) == 0) {  
        delay(10);
      }
      play_temp();
    }

    digitalWrite(16,alarm_active);

    display_adjust();
    display.setTextSize(2);
    

    display.println(timeStr);
    display.setTextSize(1);
    display.println();

    if (!(isnan(temperature) || isnan(humidity))) {
      display.print("Temperature :");
      display.print(temperature);
      display.print((char)247);
      display.println("C");
      display.println("");
      display.print("Humidity :");
      display.print(humidity);
      display.println("%");
      display.display();
    }
    if (digitalRead(25) == 0) {
      break;
    }
  }
}

int set_time() {
  display_adjust();
  display.setTextSize(2);
  display.println("Time");
  display.println(timeStr);
  display.setTextSize(1);
  display.println("Want to set time?");
  display.display();

  while(true) {
    if (digitalRead(25) == 0) {
      return 1;
    }
    else if (digitalRead(26) == 0) {
      break;
    }
  }

  display_adjust();
  display.println("Choose Hour : 0");
  display.display();

  for (int i = 1; i < 4; i++) {
    display_adjust();
    display.print("Choose ");
    display.print(labels[i-1]);
    display.print(" : ");
    display.println(time_update[i]);
    display.display();

    while(true) {
      if (digitalRead(25) == 0) {
        return 1;
      }
      else if (digitalRead(26) == 0) {
        break;
      }
      else if (digitalRead(32) == 0) {
        time_update[i] ++;
        if (time_update[i] > max_values[i-1]) {
          time_update[i] = 0;
        }
        display_adjust();
        display.print("Choose ");
        display.print(labels[i-1]);
        display.print(" : ");
        display.println(time_update[i]);
      }
      else if (digitalRead(33) == 0) {
        time_update[i] --;
        if (time_update[i] < 0) {
          time_update[i] = max_values[i-1];
        }
        display_adjust();
        display.print("Choose ");
        display.print(labels[i-1]);
        display.print(" : ");
        display.println(time_update[i]);
      }
      display.display();
    }
  }

  display_adjust();
  display.println("Save Time?");
  display.display();

  while (true) {
    if (digitalRead(26) == 0) {
      time_update[0] = 1;
      break;
    }
    else if(digitalRead(25) == 0) {
      break;
    }
  }
  delay(100);
  return 1;
}

int set_time_offset() {
  display_adjust();
  display.setTextSize(2);
  display.println("Time");
  display.println(timeStr);
  display.setTextSize(1);
  display.println("Want to set time?");
  display.display();

  while(true) {
    if (digitalRead(25) == 0) {
      return 1;
    }
    else if (digitalRead(26) == 0) {
      break;
    }
  }

  display_adjust();
  display.println("Choose GMT offset : 0");
  display.display();

  
    while(true) {
      if (digitalRead(25) == 0) {
        return 1;
      }
      else if (digitalRead(26) == 0) {
        break;
      }
      else if (digitalRead(32) == 0) {
        offset += 0.15;
        if (offset > 6.00) {
          offset = -6.00;
        }
        display_adjust();
        display.print("Choose GMT offset : ");
        display.println(offset);
      }
      else if (digitalRead(33) == 0) {
        offset -= 0.15;
        if (offset < -6.00) {
          offset = 6.00;
        }
        display_adjust();
        display.print("Choose GMT offset : ");        
        display.println(offset);
      }
      display.display();
    }

  display_adjust();
  display.println("Save Time?");
  display.display();

  while (true) {
    if (digitalRead(26) == 0) {
      break;
    }
    else if(digitalRead(25) == 0) {
      break;
    }
  }
  delay(100);
  return 1;
}

int set_alarm_1() {
  sprintf(time_alarm, "%02d:%02d:%02d", alarm_1[1], alarm_1[2], alarm_1[3]);
  display_adjust();
  display.setTextSize(2);
  display.println("Alarm 1");
  display.println(time_alarm);
  display.setTextSize(1);
  display.println("Want to set alarm 1?");
  display.display();
  
  while(true) {
    if (digitalRead(25) == 0) {
      return 1;
    }
    else if (digitalRead(26) == 0) {
      break;
    }
  }

  display_adjust();
  display.println("Choose Hour : 0");
  display.display();

  for (int i = 1; i < 4; i++) {
    display_adjust();
    display.print("Choose ");
    display.print(labels[i-1]);
    display.print(" : ");
    display.println(alarm_1[i]);
    display.display();

    while(true) {
      if (digitalRead(25) == 0) {
        return 1;
      }
      else if (digitalRead(26) == 0) {
        break;
      }
      else if (digitalRead(32) == 0) {
        alarm_1[i] ++;
        if (alarm_1[i] > max_values[i-1]) {
          alarm_1[i] = 0;
        }
        display_adjust();
        display.print("Choose ");
        display.print(labels[i-1]);
        display.print(" : ");
        display.println(alarm_1[i]);
      }
      else if (digitalRead(33) == 0) {
        alarm_1[i] --;
        if (alarm_1[i] < 0) {
          alarm_1[i] = max_values[i-1];
        }
        display_adjust();
        display.print("Choose ");
        display.print(labels[i-1]);
        display.print(" : ");
        display.println(alarm_1[i]);
      }
      display.display();
    }
  }

  display_adjust();
  display.println("Save Alarm?");
  display.display();

  while (true) {
    if (digitalRead(26) == 0) {
      alarm_1[0] = 1;
      break;
    }
    else if(digitalRead(25) == 0) {
      break;
    }
  }
  delay(100);
  return 1;
}

int set_alarm_2() {
  sprintf(time_alarm, "%02d:%02d:%02d", alarm_2[1], alarm_2[2], alarm_2[3]);
  display_adjust();
  display.setTextSize(2);
  display.println("Alarm 2");
  display.println(time_alarm);
  display.setTextSize(1);
  display.println("Want to set alarm 2?");
  display.display();

  while(true) {
    if (digitalRead(25) == 0) {
      return 1;
    }
    else if (digitalRead(26) == 0) {
      break;
    }
  }

  display_adjust();
  display.println("Choose Hour : 0");
  display.display();

  for (int i = 1; i < 4; i++) {
    display_adjust();
    display.print("Choose ");
    display.print(labels[i-1]);
    display.print(" : ");
    display.println(alarm_2[i]);
    display.display();
    Serial.println(i);

    while(true) {
      if (digitalRead(25) == 0) {
        return 1;
      }
      else if (digitalRead(26) == 0) {
        break;
      }
      else if (digitalRead(32) == 0) {
        alarm_2[i] ++;
        if (alarm_2[i] > max_values[i-1]) {
          alarm_2[i] = 0;
        }
        display_adjust();
        display.print("Choose ");
        display.print(labels[i-1]);
        display.print(" : ");
        display.println(alarm_2[i]);
      }
      else if (digitalRead(33) == 0) {
        alarm_2[i] --;
        if (alarm_2[i] < 0) {
          alarm_2[i] = max_values[i-1];
        }
        display_adjust();
        display.print("Choose ");
        display.print(labels[i-1]);
        display.print(" : ");
        display.println(alarm_2[i]);
      }
      display.display();
    }
  }

  display_adjust();
  display.println("Save Alarm?");
  display.display();

  while (true) {
    if (digitalRead(26) == 0) {
      alarm_2[0] = 1;
      break;
    }
    else if(digitalRead(25) == 0) {
      break;
    }
  }
  delay(100);
  return 0;
}

void go_to_menu(int select, String menu_items[], int size) {
  display_adjust();
  display.println("MENU");
  display.setTextSize(1);
  for (int i = 0; i < 5; i++) {
    if (i != select) {
      display.setTextColor(WHITE);     
    }
    else {
      display.setTextColor(BLACK,WHITE);
    }
    display.println(menu_items[i]);
  }
  display.display();
}

void update_menu() {
  if (digitalRead(32) == 0) {
    if (item == menu_size-1) {
      item = 0;
    }
    else {
      item += 1;
    }
    go_to_menu(item, menu, menu_size);
  }
  else if (digitalRead(33) == 0) { 
    if (item == 0) {
      item = menu_size - 1;
    }
    else {
      item -= 1;
    }
    go_to_menu(item, menu, menu_size);
  }
}

void remove_alarm() {
  display_adjust();
  display.println("Delete Alarm 1?");
  display.display();

  while(true) {
    if (digitalRead(26) == 0) {
      for (int i = 0; i < 4; i++) {
        alarm_1[i] = 0;
        alarm_2[i] = 0;
      }
      break;
    }
    else if (digitalRead(25) == 0) {
      break;
    }
  }

  display_adjust();
  display.println("Delete Alarm 2?");
  display.display();

  while(true) {
    if (digitalRead(26) == 0) {
      for (int i = 0; i < 4; i++) {
        alarm_1[i] = 0;
        alarm_2[i] = 0;
      }
      break;
    }
    else if (digitalRead(25) == 0) {
      break;
    }
  }
  delay(100);
}

void display_adjust() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.setTextColor(WHITE);
}

void color(int colour) {
  if (colour == 0) {
    display.setTextColor(WHITE);
  }
  else {
    display.setTextColor(BLACK, WHITE);
  }
}