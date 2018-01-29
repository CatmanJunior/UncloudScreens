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

const byte BUTTONPIN1 = 14;   // Connect button1 to pin D5
const byte BUTTONPIN2 = 12;   // Connect button2 to pin D6
const byte FADERPIN1 = 5;     // Connect fader1 to pin D1
const byte FADERPIN2 = 4;     // Connect fader2 to pin D2
const byte FADERPIN3 = 13;    // Connect fader to pin D7
const byte LEDPIN = 0;        // Connect LED to pin D3

const byte NUMPIXELS = 1;    //Amount of LEDS

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

// WiFi
// Make sure to update this for your own WiFi network!
const char* SS_ID = "test";
const char* WIFI_PASSWORD = "testtest";
const char* TEST_SS_ID = "Ziggo78F5D45";
const char* TEST_WIFI_PASSWORD = "Sx7phx8fnkeP";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* MQTT_SERVER = "192.168.178.194";
const char* MQTT_TOPIC_BUTTON = "c2_b1";
const char* MQTT_TOPIC_FADER[3] = {"c2_f1", "c2_f2", "c2_f3"};
const char* MQTT_USERNAME = "uncloud";
const char* MQTT_PASSWORD = "uncloud";
const char* CLIENT_ID = "controller2"; // The client id identifies the ESP8266 device. Think of it a bit like a hostname.

char msgBuffer[20];

//// Initialise the Bouncer objects
Bounce BUTTON[2] = {Bounce(), Bounce()};
byte FADER[3] = {FADERPIN1, FADERPIN2, FADERPIN3};

int pressedButton = 0;

int faderValue[3] = {0, 0, 0};

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

void setup() {
  Serial.begin(115200);

  pixels.begin(); // This initializes the NeoPixel library.

  pinMode(FADERPIN1, OUTPUT);
  pinMode(FADERPIN2, OUTPUT);
  pinMode(FADERPIN3, OUTPUT);

  pinMode(BUTTONPIN1, INPUT);
  pinMode(BUTTONPIN2, INPUT);

  // Setup pushbutton Bouncer object
  BUTTON[0].attach(BUTTONPIN1);
  BUTTON[1].attach(BUTTONPIN2);

  for (int i = 0; i < 2; i++) {
    BUTTON[i].interval(5);
  }

  pixelsOneColor(100, 155, 000);
  //Wait a minute before starting up so we are sure the Raspberry has started (Cancel by pressing button)
  while ((digitalRead(BUTTONPIN1) == LOW) && (millis() < 60000)) {
    delay(50);
  }

  delay(500);

  //WIFI INIT
  Serial.print("Connecting to ");
  Serial.println(SS_ID);

  // Connect to the WiFi (if button is being hold, use test wifi)
  if (digitalRead(BUTTONPIN1) == LOW) WiFi.begin(SS_ID, WIFI_PASSWORD);
  else WiFi.begin(TEST_SS_ID, TEST_WIFI_PASSWORD);

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
  delay(500);
  pixelsOneColor(0, 0, 0);

  // Connect to MQTT Broker
  if (client.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.println("Connected to MQTT Broker!");
  } else {
    Serial.println("Connection to MQTT Broker failed...");
    pixelsOneColor(0, 155, 155); //if no connection turn purple
  }
}

void loop() {
  pressedButton = 0; //Reset the pressedButton

  for (int i = 0; i < 3; i++) {
    digitalWrite(FADER[i], HIGH);
    delay(5);
    faderValue[i] = analogRead(A0);
//    faderValue[i] = log10(analogRead(A0)) * 323;
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

  if (pressedButton != 0) {
    sprintf(msgBuffer, "%02d", pressedButton);
    Serial.println(msgBuffer);
    // PUBLISH to the MQTT Broker
    if (client.publish(MQTT_TOPIC_BUTTON, msgBuffer)) {
      Serial.println("Button pushed and message sent!");
    }
    else { //if it failed, try again
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
      client.publish(MQTT_TOPIC_BUTTON, msgBuffer);
    }
  }

  for (int i = 0; i < 2; i++) {
    sprintf(msgBuffer, "%04d", faderValue[0]);
    client.publish(MQTT_TOPIC_FADER[0], msgBuffer);
  }

  //  Uncomment these lines for debugging
  Serial.println("FA1:" + String(faderValue[0]));
  Serial.println("FA2:" + String(faderValue[1]));
  Serial.println("FA3:" + String(faderValue[2]));
  Serial.println("BT:" + String(pressedButton));

  delay(50);
}

