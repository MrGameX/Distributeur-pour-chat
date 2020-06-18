
// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Time.h>
#include "RTClib.h"
#include <Wire.h>

RTC_DS3231 rtc;

// Replace with your network credentials
const char* ssid     = "Ap";
const char* password = "12345678";


//const char* ssid     = "ProjetTFE";
//const char* password = "Azerty123";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Set LED GPIO
const int ledPin = 12;
const int pump = 14;
const int engine = 27;
// Stores LED state
String ledState;
bool pumpState = false;
bool engineState = false;

String formattedDate;
String progTime[64];
String progMode[64];
int nbrProg = 0;

String heures;
String minutes;
bool eau;
bool croq;

int a, m, d, h, n, s;

String alert = "";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  if(var == "time1"){
    if(progMode[0] != "" and progTime[0] != ""){
      return String(progMode[0] + " - " + progTime[0]);
    }else{
      return "";
    }
      
  }
  if(var == "time2"){
    if(progMode[1] != "" and progTime[1] != ""){
      return String(progMode[1] + " - " + progTime[1]);
    }else{
      return "";
    }
  }
  if(var == "time3"){
    if(progMode[2] != "" and progTime[2] != ""){
      return String(progMode[2] + " - " + progTime[2]);
    }else{
      return "";
    }
  }
  if(var == "time4"){
    if(progMode[3] != "" and progTime[3] != ""){
      return String(progMode[3] + " - " + progTime[3]);
    }else{
      return "";
    }
  }
  if(var == "time5"){
    if(progMode[4] != "" and progTime[4] != ""){
      return String(progMode[4] + " - " + progTime[4]);
    }else{
      return "";
    }
  }
  if(var == "alert"){
    return String(alert);
  }
  if(var == ""){
    String percent = "%";
    return percent;
  }
  return String();
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(engine, OUTPUT);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //rtcMod = false;
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date &amp; time this sketch was compiled
    setRTCTime();
    // This line sets the RTC with an explicit date &amp; time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  //WiFi.softAP(ssid, password);

  //IPAddress IP = WiFi.softAPIP();
  

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  //Serial.print("AP IP address: ");
  //Serial.println(IP);

  timeClient.begin();
  timeClient.setTimeOffset(7200);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/prog.html", HTTP_GET, [](AsyncWebServerRequest *request){

    int paramsNr = request->params();
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
        
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");
        Serial.println(p->name());
        Serial.print("Param value: ");
        Serial.println(p->value());
        Serial.println("------");
        if(p->name() == "h"){
          heures = p->value();
        }
        else if(p->name() == "m"){
          minutes = p->value();
        }
        else if(p->name() == "eau"){
          eau = true;
        }
        else if(p->name() == "croq"){
          croq = true;
        }
        
    }
    if(heures != "" and minutes != "" and (eau == true or croq == true)){
      Serial.println("timer");
      String ProgTimer = heures+"h"+minutes;
      Serial.println(ProgTimer);
      if(nbrProg <=4 ){
        progTime[nbrProg]= ProgTimer;
        alert = "Timer ajouté !";
        Serial.println(progTime[0]);
        Serial.println(progTime[1]);
        if(eau == true and croq != true){
          progMode[nbrProg] = "eau";
        }
        else if(croq == true and eau != true){
          progMode[nbrProg] = "croq";
        }
        else if(eau == true and croq == true){
          progMode[nbrProg] = "both";
        }
        eau=false;
        croq=false;
        nbrProg++;
      }else{
        alert = "Le nombre de timer max = 5";
      }
      
    }else{
      alert = "Votre configuration est incomplète"; 
    }
    
    request->send(SPIFFS, "/prog.html", String(), false, processor);
  });

  server.on("/manuel.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/manuel.html", String(), false, processor);
  });

  server.on("/infos.html", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/infos.html", String(), false, processor);
  })

  server.on("/claw.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/claw.ico", "image/ico");
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/all.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/all.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/led.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/led.html", String(), false, processor);
  });

  server.on("/eau", HTTP_GET, [](AsyncWebServerRequest *request){
    pumpFunc();   
    request->send(SPIFFS, "/manuel.html", String(), false, processor);
  });

  server.on("/croq", HTTP_GET, [](AsyncWebServerRequest *request){
    engineFunc();   
    request->send(SPIFFS, "/manuel.html", String(), false, processor);
  });

  setRTCTime();
  // Start server
  server.begin();
}

void pumpFunc(){

  digitalWrite(pump, HIGH);
  delay(3000);
  digitalWrite(pump,LOW);
  
//    if(!pumpState){
//      digitalWrite(pump, HIGH);
//      pumpState = true;
//    }
//    else{
//      digitalWrite(pump, LOW);
//      pumpState = false;
//      }
  }

void engineFunc(){

  digitalWrite(engine, HIGH);
  delay(7000);
  digitalWrite(engine,LOW);
  
//    if(!engineState){
//      digitalWrite(engine, HIGH);
//      engineState = true;
//    }
//    else{
//      digitalWrite(engine, LOW);
//      engineState = false;
//      }
  }

void setRTCTime(){
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  formattedDate = timeClient.getFormattedDate();

  a = formattedDate.substring( 0, 4).toInt();

  m = formattedDate.substring( 5, 7).toInt();
  
  d = formattedDate.substring( 8, 10).toInt();
  
  h = formattedDate.substring(11, 13).toInt();
  
  n = formattedDate.substring(14, 16).toInt();
  
  s = formattedDate.substring(17, 19).toInt();
  
  rtc.adjust(DateTime(a, m, d, h, n, s));
}

void turnLed(String out){
  if(out == "eau"){
    digitalWrite(pump, HIGH);
    delay(5000);
    digitalWrite(pump, LOW);
  }
  else if(out == "croq"){
    digitalWrite(engine, HIGH);
    delay(5000);
    digitalWrite(engine, LOW);
  }
  else if(out == "both"){
    digitalWrite(pump, HIGH);
    digitalWrite(engine, HIGH);
    delay(5000);
    digitalWrite(pump, LOW);
    digitalWrite(engine, LOW);
  }
}

void checkProg(void){
  DateTime now = rtc.now();
  
  int test1 = now.hour();
  int test2 = now.minute();
  int test3 = now.second();

  Serial.print("Il est: ");
  Serial.print(test1);
  Serial.print("h");
  Serial.println(test2);
  
  for(int number = 0; number < nbrProg; number++){
    int index = progTime[number].indexOf("h");
    int hou = progTime[number].substring(0, 2).toInt();
    int mi = progTime[number].substring(index+1, index+3).toInt();
    String out = progMode[number];
    Serial.println(hou);
    Serial.println(mi);

    if(test1 == hou and test2 == mi and test3 == 0){
      Serial.println("C'est l'heure");
      turnLed(out);
    }
  }
}
 
void loop(){

  

  checkProg();

  delay(1000);
  
}
