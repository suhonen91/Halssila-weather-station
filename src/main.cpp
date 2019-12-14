#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define SLEEP_TIME 6000000
#define SQL_LED 19
#define WIFI_LED 18
#define ERROR_LED 2

int lastErrorCause = 0;
bool wifiBlink = true;


IPAddress server_addr(192,168,0,25);  // IP of the MySQL *server* here
char user[] = "test";              // MySQL user login username
char password[] = "password";        // MySQL user login password

// Sample query
char INSERT_SQL[] = "INSERT INTO weathertest1.weatherData (location, Temperature, Pressure, Humidity) VALUES (%d,%f,%f,%f)";
char query[128];

// WiFi card example
char ssid[] = "Vanilla wireless";         // your SSID
char pass[] = "vanillataste";     // your SSID Password

WiFiClient client;             // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

//unsigned long delayTime;

float currentTemperature;
float currentPressure;
float currentHumidity;


void setup() {
  Serial.begin(115200);

  pinMode(WIFI_LED,OUTPUT);
  pinMode(SQL_LED,OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  digitalWrite(WIFI_LED, LOW);
  digitalWrite(SQL_LED, LOW);
  digitalWrite(ERROR_LED, LOW);

  // Begin WiFi section
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (wifiBlink){digitalWrite(ERROR_LED, LOW);}
    else{digitalWrite(ERROR_LED, HIGH);}
    wifiBlink = !wifiBlink;
    //WiFi.reconnect();
    }
  digitalWrite(ERROR_LED, LOW);
  if (WiFi.status()!= WL_CONNECTED){
    lastErrorCause = 1;
  }

  esp_sleep_enable_timer_wakeup(SLEEP_TIME);

 
  if (conn.connect(server_addr, 3306, user, password)){
    
  }
  else {
    //digitalWrite(ERROR_LED, HIGH);
    lastErrorCause = 2;
  }
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
  Serial.println(F("BME280 test"));

  bool status;

  status = bme.begin(0x76);  
  if (!status) {
    //digitalWrite(ERROR_LED, HIGH);
    lastErrorCause = 3;
    Serial.println(lastErrorCause);
    Serial.println(status); 
    }
}
void readValues() {
  //Serial.print("Temperature = ");
  currentTemperature = bme.readTemperature();
  //Serial.print(currentTemperature);
  //Serial.println(" *C");
  
  //Serial.print("Pressure = ");
  currentPressure = bme.readPressure() / 100.0F;
  //Serial.print(currentPressure);
  //Serial.println(" hPa");

  //Serial.print("Humidity = ");
  currentHumidity = bme.readHumidity();
  //Serial.print(currentHumidity);
  //Serial.println(" %");

  //Serial.println();
}


void loop() { 
  readValues();
  if (conn.connected() && lastErrorCause == 0){
    sprintf(query,INSERT_SQL,1, currentTemperature, currentPressure, currentHumidity);
    cursor->execute(query);
    digitalWrite(WIFI_LED, HIGH);
    delay(100);
    digitalWrite(WIFI_LED, LOW);
  }
  else{
    //error cases here.
    //digitalWrite(ERROR_LED, HIGH);

    switch (lastErrorCause)
    {  
    case 1:
      digitalWrite(ERROR_LED,HIGH);
      delay(500);
      digitalWrite(ERROR_LED,LOW);
      break;
    case 2:
      digitalWrite(ERROR_LED,HIGH);
      delay(500);
      digitalWrite(ERROR_LED,LOW);
      delay(500);
      digitalWrite(ERROR_LED,HIGH);
      delay(500);
      digitalWrite(ERROR_LED,LOW);
      break;
    case 3:
      digitalWrite(ERROR_LED,HIGH);
      delay(500);
      digitalWrite(ERROR_LED,LOW);
      delay(500);
      digitalWrite(ERROR_LED,HIGH);
      delay(500);
      digitalWrite(ERROR_LED,LOW);
      delay(500);     
      digitalWrite(ERROR_LED,HIGH);
      delay(500);
      digitalWrite(ERROR_LED,LOW);
      break;
    
    default:
      while(true){
        delay(100);
        digitalWrite(ERROR_LED,HIGH);
        delay(100);
        digitalWrite(ERROR_LED,LOW);
      }
      break;
    }

  }
  Serial.println("going sleep");
  digitalWrite(WIFI_LED, LOW);
  digitalWrite(SQL_LED, LOW);
  esp_deep_sleep_start();
}

