/*
   ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Publish Example
   Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
   Made as part of my MQTT Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
*/
#include <Bounce2.h> // Used for "debouncing" the pushbutton
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <Adafruit_NeoPixel.h> //Lib for the programable leds
#include <math.h>

const byte BUTTONPIN1 = 5;   // Connect button1 to pin D5
const byte BUTTONPIN2 = 4;   // Connect button2 to pin D6
const byte FADERPIN1 = 12;     // Connect fader1 to pin D1
const byte FADERPIN2 = 13;    // Connect fader to pin D7
const byte LEDPIN = 14;        // Connect LED to pin D3

const byte NUMPIXELS = 2;    //Amount of LEDS

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_RGB + NEO_KHZ800);

// WiFi
// Make sure to update this for your own WiFi network!
const char* SS_ID = "test";
const char* WIFI_PASSWORD = "testtest";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* MQTT_SERVER = "192.168.15.102";
const char* MQTT_TOPIC_BUTTON = "c2_b1";
const char* MQTT_TOPIC_FADER[3] = {"c2_f1", "c2_f2", "c2_f3"};
const char* MQTT_USERNAME = "uncloud";
const char* MQTT_PASSWORD = "uncloud";
const char* CLIENT_ID = "controller3"; // The client id identifies the ESP8266 device. Think of it a bit like a hostname.

char msgBuffer[20];

//// Initialise the Bouncer objects
Bounce BUTTON[2] = {Bounce(), Bounce()};
byte FADER[2] = {FADERPIN1, FADERPIN2};

int pressedButton = 0;

int faderValue[2] = {0, 0};

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(MQTT_SERVER, 1883, wifiClient); // 1883 is the listener port for the Broker

//Method for setting all pixels one color
void pixelsOneColor(int r, int g, int b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
}

void startConnection()
{
  pixelsOneColor(100, 155, 000);
  //Wait a minute before starting up so we are sure the Raspberry has started (Cancel by pressing button)
  while ((digitalRead(BUTTONPIN1) == LOW) && (millis() < 60000)) {
    delay(50);
  }

  //WIFI INIT
  Serial.print("Connecting to ");
  Serial.println(SS_ID);

  // Connect to the WiFi (if button is being hold, use test wifi)
  WiFi.begin(SS_ID, WIFI_PASSWORD);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    pixelsOneColor(155, 0, 0);
    delay(250);
    Serial.print(".");
    pixelsOneColor(0, 0, 0);
    delay(250);
  }

  pixelsOneColor(0, 155, 0);
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  if (client.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.println("Connected to MQTT Broker!");
    pixelsOneColor(0, 0, 0);
  } else {
    Serial.println("Connection to MQTT Broker failed...");
    pixelsOneColor(0, 155, 155); //if no connection turn purple
    delay(5000);
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);

  pixels.begin(); // This initializes the NeoPixel library.

  pinMode(FADERPIN1, OUTPUT);
  pinMode(FADERPIN2, OUTPUT);
  pinMode(BUTTONPIN1, INPUT);
  pinMode(BUTTONPIN2, INPUT);

  // Setup pushbutton Bouncer object
  BUTTON[0].attach(BUTTONPIN1);
  BUTTON[1].attach(BUTTONPIN2);

  for (int i = 0; i < 2; i++) {
    BUTTON[i].interval(5);
  }

//  startConnection();
}

void loop() {
  pressedButton = 0; //Reset the pressedButton

  for (int i = 0; i < 2; i++) {
    digitalWrite(FADER[i], HIGH);
    delay(5);
    faderValue[i] = analogRead(A0);
    //    faderValue[i] = int(log10(float(analogRead(A0) - 4)) * 331.0);
    digitalWrite(FADER[i], LOW);
  }

  for (int i = 0; i < 2; i++) {
    BUTTON[i].update();     //Update the button state
    if (BUTTON[i].rose()) { //If it went from low to high
      pressedButton = i + 1;
      if (i == 0) pixels.setPixelColor(0, pixels.Color(0, 150, 0)); // Moderately bright green color.
      if (i == 1) pixels.setPixelColor(0, pixels.Color(150, 0, 0)); // Moderately bright green color.
    }
    if (BUTTON[i].fell()) { //if it went from high to low
      pressedButton = i + 3;
    }
  }

  pixels.show(); // This sends the updated pixel color to the hardware.
//
//  if (pressedButton != 0) {
//    sprintf(msgBuffer, "%02d", pressedButton);
//    Serial.println(msgBuffer);
//    // PUBLISH to the MQTT Broker
//    if (client.publish(MQTT_TOPIC_BUTTON, msgBuffer)) {
//      Serial.println("Button pushed and message sent!");
//    }
//    else { //if it failed, try again
//      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
//      client.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
//      client.publish(MQTT_TOPIC_BUTTON, msgBuffer);
//    }
//  }
//
//  for (int i = 0; i < 3; i++) {
//    sprintf(msgBuffer, "%04d", faderValue[i]);
//    client.publish(MQTT_TOPIC_FADER[i], msgBuffer);
//  }

//    Uncomment these lines for debugging
    Serial.println("FA1:" + String(faderValue[0]));
    Serial.println("FA2:" + String(faderValue[1]));

    Serial.println("BT:" + String(pressedButton));

  delay(150);
}

