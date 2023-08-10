//Initializing Blynk app
#define BLYNK_TEMPLATE_ID           "TMPL69FYO9klc"                         //Blynk templete id
#define BLYNK_TEMPLATE_NAME         "new project"                           //Blynk template name
#define BLYNK_AUTH_TOKEN            "x1opGYC14M9To1j4HwuPKCHWvUQEsDae"      //Blynk auth code

// Importing nesessory libraries
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi credentials
char ssid[] = "Routz";
char pass[] = "12341234";

// Initializing Servo Motor
Servo servo1, servo2;
int switch_state = 0;

// Initializing Power Cut Handler
int sensorValue = 0;
int sensorPin = A0;

#define BLYNK_PRINT Serial

// Time functions

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "lk.pool.ntp.org");

void time_handle(){
  timeClient.update();
  timeClient.setTimeOffset(19800);
  int HH = timeClient.getHours();
  int MM = timeClient.getMinutes();
  int SS = timeClient.getSeconds();
  setTime(HH, MM, SS, 8, 10, 23);

}

BLYNK_CONNECTED() {
  Blynk.sendInternal("rtc", "iso");
  Serial.println("connected");
  Serial.println("connected");
}

// Control Servo  
BLYNK_WRITE(V0){
  switch_state = param.asInt();
  if(switch_state == 0){
    turn_off();
  }
  else{
    turn_on();
  }
}

void turn_off(){
  servo1.write(180);
  delay(500);
  servo1.write(0);
  Serial.println("off");
}

void turn_on(){
  servo2.write(0);
  delay(500);
  servo2.write(180);
  Serial.println("on");
}

// Alarm Functions
void alarm(int hh, int mm, int ss, int status){
  if(status == 0){
    Alarm.alarmRepeat(hh,mm,ss,turn_off);
  }else if(status == 1){
    Alarm.alarmRepeat(hh,mm,ss,turn_on);
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
      turn_off();
      set_to_deafult = 1;
      Serial.println("set to default 0");

    }else{
      turn_on();
      set_to_deafult = 1;
      Serial.println("set to default 1");
    }
  }else if(sensorValue == 1024){
    set_to_deafult = 0;
    Serial.println("reset default");
  }
  Serial.print("sensorValue ");
  Serial.println(sensorValue);
  Serial.print("set_to_deafult ");
  Serial.println(set_to_deafult);
}

void setup(){
  // Debug console
  Serial.begin(115200);

  // Connecting to Server 
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80); 
  
  // Set Alarms
  time_handle();
  alarm(18,2,0, 1);
  alarm(18,2,10, 0);
  alarm(18,2,15, 1);
  alarm(18,2,20, 0);

  // Initial Servo1 Position
  servo1.attach(2);
  servo1.write(0); 

  // Initial Servo2 Position
  servo2.attach(0);
  servo2.write(180); 
}

void loop(){
  Blynk.run();
  Alarm.delay(200);
  power_cut_handle(0);
  delay(1500);
  Serial.println(analogRead(sensorPin));
}