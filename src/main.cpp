#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>


#define SEALEVELPRESSURE_HPA (1013.25)

IPAddress server_addr(192,168,0,2);  // IP of the MySQL *server* here
char user[] = "test1";              // MySQL user login username
char password[] = "password";        // MySQL user login password

// Sample query
char INSERT_SQL[] = "INSERT INTO weather test 1.weatherdata (location, Temperature, Pressure, Humidity) VALUES (%d,%f,%f,%f)";
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

unsigned long delayTime;

float currentTemperature;
float currentPressure;
float currentHumidity;


void setup() {
  Serial.begin(115200);

  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

    // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
  Serial.println(F("BME280 test"));

  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();
}
void printValues() {
  Serial.print("Temperature = ");
  currentTemperature = bme.readTemperature();
  Serial.print(currentTemperature);
  Serial.println(" *C");
  
  // Convert temperature to Fahrenhei
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/
  
  Serial.print("Pressure = ");
  currentPressure = bme.readPressure() / 100.0F;
  Serial.print(currentPressure);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  currentHumidity = bme.readHumidity();
  Serial.print(currentHumidity);
  Serial.println(" %");

  Serial.println();
}


void loop() { 
  printValues();
  if (conn.connected())
    sprintf(query,INSERT_SQL,1, currentTemperature, currentPressure, currentHumidity);
    cursor->execute(query);
  delay(delayTime);
}

