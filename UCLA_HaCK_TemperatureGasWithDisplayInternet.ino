#include <Wire.h>
#include "HackPublisher.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_AM2320 am2320 = Adafruit_AM2320();

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins for gas sensor
const int gasSensorAnalog = A3;
const int gasSensorOutput = A4;

// Pins for ultrasonic sensor
const int trigPin = 21;
const int echoPin = A2;

// Internet settings
const char *ssid = "ASUS-F8";
const char *password = "K33pi7$@f3%";

HackPublisher publisher("J.Y.M.", true);

void setup() {
  Serial.begin(115200);
  am2320.begin();
  pinMode(gasSensorAnalog, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  // Start publisher
  publisher.begin();

  Serial.println("OK");
}

void loop() {

  // ---Humidity and temperature---
  float humidity = am2320.readHumidity();
  delay(500);
  float temperature = am2320.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // -------Gas sensor--------
  int gasSensorValue = analogRead(gasSensorAnalog); // Get gas sensor value
  Serial.print("Gas: ");
  Serial.println(gasSensorValue);

  // ------Ultrasonic sensor-----
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH State for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time
  int duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  int distance = (duration * 0.034) / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  

  // ----OLED settings, display----
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.display();
  oled.setTextColor(WHITE);
  oled.setTextSize(2);
  oled.setTextWrap(true);
  
  oled.print("Temp:");
  oled.print(temperature, 1);
  oled.println("C");
  oled.print("Hum:");
  oled.print(humidity, 1);
  oled.println("%");
  oled.print("Gas:");
  oled.println(gasSensorValue);
  displayDanger(gasSensorValue, temperature, humidity);

  // Upload data to the website
  publisher.store("temperature", temperature);
  publisher.store("theGasData", gasSensorValue);
  publisher.store("humid", humidity);
  publisher.store("distance", distance);
  
  publisher.send();
  Serial.println("Sent data...");
  
  oled.display();
  delay(1000);
}



// Displays danger on the OLED if any sensor
// reads too high/low
void displayDanger(int gas,float temp,float humid){
  if(gas > 100 || temp > 24 || humid < 45){
    oled.print("DNG:");
    if(gas > 3050){ // CHANGE!!
      oled.print("G ");
    }
    if(temp > 32){
      oled.print("T ");
    }
    if(humid < 45){
      oled.print("H");
    }
  }



  
}
