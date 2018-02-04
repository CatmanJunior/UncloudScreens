/*
   ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Publish Example
   Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
   Made as part of my MQTT Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
*/
#include <Bounce2.h> // Used for "debouncing" the pushbutton
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <Adafruit_NeoPixel.h>

const byte BUTTONPIN1 = 14; // Connect your button to pin D5
const byte BUTTONPIN2 = 12; // Connect your button to pin #13
const byte XPIN = 5; // Connect your button to pin #13
const byte YPIN = 4; // Connect your button to pin #13
const byte LEDPIN = 0;

int XValue;
int YValue;

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      2

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_RGB + NEO_KHZ800);

// WiFi
// Make sure to update this for your own WiFi network!
const char* SS_ID = "test";
const char* WIFI_PASSWORD = "testtest";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* MQTT_SERVER = "192.168.15.102";
const char* MQTT_TOPIC_X = "c3_x1";
const char* MQTT_TOPIC_Y = "c3_y1";
const char* MQTT_TOPIC_BUTTON = "c3_b1";
const char* MQTT_USERNAME = "uncloud";
const char* MQTT_PASSWORD = "uncloud";
const char* CLIENT_ID = "controller3"; // The client id identifies the ESP8266 device. Think of it a bit like a hostname.

char msgBuffer[20];

// Initialise the Pushbutton Bouncer object
Bounce BUTTON[2] = {Bounce(), Bounce()};

int pressedButton = 0;

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

  pinMode(XPIN, OUTPUT);
  pinMode(YPIN, OUTPUT);

  // Setup pushbutton Bouncer object
  pinMode(BUTTONPIN1, INPUT);
  BUTTON[0].attach(BUTTONPIN1);
  BUTTON[0].interval(5);

  pinMode(BUTTONPIN2, INPUT);
  BUTTON[1].attach(BUTTONPIN2);
  BUTTON[1].interval(5);

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

void loop() {
  digitalWrite(XPIN, HIGH);
  XValue = analogRead(A0);
  digitalWrite(XPIN, LOW);
  delay(5);
  digitalWrite(YPIN, HIGH);
  YValue = analogRead(A0);
  digitalWrite(YPIN, LOW);

  int pressedButton = 0;
  // Update button state
  BUTTON[0].update();
  BUTTON[1].update();

  if (BUTTON[0].rose()) {
    pressedButton = 1;
    pixels.setPixelColor(0, pixels.Color(0, 150, 0)); // Moderately bright green color.
  }
  if (BUTTON[1].rose()) {
    pressedButton = 2;
    pixels.setPixelColor(0, pixels.Color(150, 0, 0)); // Moderately bright green color.
  }

  pixels.show(); // This sends the updated pixel color to the hardware.

  if (pressedButton != 0) {
    // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
    // Here, "Button pressed!" is the Payload, but this could be changed to a sensor reading, for example.
    sprintf(msgBuffer, "%02d", pressedButton);
    if (client.publish(MQTT_TOPIC_BUTTON, msgBuffer)) {
      Serial.println("Button pushed and message sent!");
    }
    else {
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
    }
  }


  sprintf(msgBuffer, "%04d", XValue);
  client.publish(MQTT_TOPIC_X, msgBuffer);
  sprintf(msgBuffer, "%04d", YValue);
  client.publish(MQTT_TOPIC_Y, msgBuffer);
  //  Serial.println("Sw1:" + String(faderValue1));
  //  Serial.println("Sw2:" + String(faderValue2));
  //  Serial.println("Sw3:" + String(faderValue3));
  Serial.println(pressedButton);

  delay(150);
}

