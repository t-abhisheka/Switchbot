//Initializing Blynk app
#define BLYNK_TEMPLATE_ID           ""
#define BLYNK_TEMPLATE_NAME         ""
#define BLYNK_AUTH_TOKEN            ""

// Importing nesessory libraries
#include <ESP8266WiFi.h>              // For Board Control
#include <BlynkSimpleEsp8266.h>       // For Link Board With BLYNK App
#include <Servo.h>                    // Control Servo Motor
#include <Time.h>                     // For Time Keeping
#include <TimeAlarms.h>               // For Alarms
#include <NTPClient.h>                // For Time Keeping
#include <WiFiUdp.h>                  // For Time Keeping

// WiFi Credentials
char ssid[] = "Routz";
char pass[] = "12341234";

// Initializing Servo Motor
Servo servo;
int switch_state = 0;

// Initializing Power Cut Handler
int sensorValue = 0;
int sensorPin = A0;

// Initializing Serial
#define BLYNK_PRINT Serial

// Connect to NTP Server
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "lk.pool.ntp.org");

// Time Handler
void time_handle(){
  timeClient.update();
  timeClient.setTimeOffset(19800);
  int HH = timeClient.getHours();
  int MM = timeClient.getMinutes();
  int SS = timeClient.getSeconds();
  setTime(HH, MM, SS, 8, 10, 23);
}

// Getting time from BLYNK Server
BLYNK_CONNECTED() {
  Blynk.sendInternal("rtc", "iso");
  Serial.println("");
  Serial.println("Connected to the Server");
}

// Control Servo  
BLYNK_WRITE(V0){
  switch_state = param.asInt();
  if(switch_state == 0){
    turn_off();
    Serial.println("Switch was Turned Off Through The App or Google Assistant");
  }else{
    turn_on();
    Serial.println("Switch was Turned On Through The App or Google Assistant");
  }
}

// Servor Turn Off Function
void turn_off(){
  servo.write(0);
}

// Servor Turn On Function
void turn_on(){
  servo.write(60);
}

// Alarm Functions
void alarm(int hh, int mm, int ss, int status){
  if(status == 0){
    Alarm.alarmRepeat(hh,mm,ss,turn_off);
    Serial.print("Switch will Off at ");
    Serial.print(hh);
    Serial.print(":");
    Serial.print(mm);
    Serial.print(":");
    Serial.println(ss);
  }else if(status == 1){
    Alarm.alarmRepeat(hh,mm,ss,turn_on);
    Serial.print("Switch will On at ");
    Serial.print(hh);
    Serial.print(":");
    Serial.print(mm);
    Serial.print(":");
    Serial.println(ss);
  }
}

// Power Cut Handler
int set_to_deafult = 0;
void power_cut_handle(int default_value){
  sensorValue = analogRead(sensorPin);
  if(sensorValue != 1024){
    sensorValue = 0;
  }
  // sensor_value 1024 = have electracity
  // sensor_value 0 = havn't electracity

  if(sensorValue == 0 && set_to_deafult ==0){
    if(default_value == 0){
      Serial.print("Sensor Value : ");
      Serial.println(sensorValue);
      turn_off();
      Serial.println("Switch was Turned Off by Power Cut Handler");
      set_to_deafult = 1;
    }else{
      Serial.print("Sensor Value : ");
      Serial.println(sensorValue);
      turn_on();
      Serial.println("Switch was Turned On by Power Cut Handler");
      set_to_deafult = 1;
    }
  }else if(sensorValue == 1024){
    set_to_deafult = 0;
  }
}

void setup(){
  // Debug console
  Serial.begin(115200);

  // Connecting to Server 
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80); 
  
  // Set Alarms
  time_handle();
  alarm(12,57,0, 1);
  alarm(12,57,10, 0);
  alarm(12,57,15, 1);
  alarm(12,57,20, 0);

  // Initial Servo
  servo.attach(2,500,2500);
}

void loop(){
  Blynk.run();                              // Run BYLNK app
  Alarm.delay(200);                         // Run Alarms
  power_cut_handle(0);                      // Run Power Cut Handler
}
