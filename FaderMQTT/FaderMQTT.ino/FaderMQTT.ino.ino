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
const byte BUTTONPIN2 = 12; // Connect your button to pin D6
const byte FADERPIN1 = 5; // Connect your fader to pin D1
const byte FADERPIN2 = 4; // Connect your fader to pin D2
const byte FADERPIN3 = 13; // Connect your fader to pin D7
const byte LEDPIN = 0; //Connect your led to pin D3

char msgBuffer[20];
int faderValue1;
int faderValue2;
int faderValue3;

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      16

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

// WiFi
// Make sure to update this for your own WiFi network!
const char* ssid = "Ziggo78F5D45";
const char* wifi_password = "Sx7phx8fnkeP";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "192.168.178.194";
const char* mqtt_topic1 = "button2";
const char* mqtt_topic2 = "fader1";
const char* mqtt_topic3 = "fader2";
const char* mqtt_topic4 = "fader3";
const char* mqtt_username = "uncloud";
const char* mqtt_password = "uncloud";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "controller1";

// Initialise the Pushbutton Bouncer object
Bounce button1 = Bounce();
Bounce button2 = Bounce();

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  pinMode(FADERPIN1, OUTPUT);
  pinMode(FADERPIN2, OUTPUT);
  pinMode(FADERPIN3, OUTPUT);

  pinMode(BUTTONPIN1, INPUT);
  pinMode(BUTTONPIN2, INPUT);

  // Setup pushbutton Bouncer object
  button1.attach(BUTTONPIN1);
  button1.interval(5);
  button2.attach(BUTTONPIN2);
  button2.interval(5);

  // Begin Serial on 115200
  // Remember to choose the correct Baudrate on the Serial monitor!
  // This is just for debugging purposes
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  pixels.setPixelColor(0, pixels.Color(0, 0, 0)); // Moderately bright green color.
  pixels.show(); // This sends the updated pixel color to the hardware.

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void loop() {
  digitalWrite(FADERPIN1, HIGH);
  faderValue1 = analogRead(A0);
  digitalWrite(FADERPIN1, LOW);
  delay(5);
  digitalWrite(FADERPIN2, HIGH);
  faderValue2 = analogRead(A0);
  digitalWrite(FADERPIN2, LOW);
  delay(5);
  digitalWrite(FADERPIN3, HIGH);
  faderValue3 = analogRead(A0);
  digitalWrite(FADERPIN3, LOW);
  delay(5);

  int pressedButton = 0;
  // Update button state
  button1.update();
  button2.update();

  if (button1.rose()) {
    pressedButton = 1;
    pixels.setPixelColor(0, pixels.Color(0, 150, 0)); // Moderately bright green color.
  }
  if (button2.rose()) {
    pressedButton = 2;
    pixels.setPixelColor(0, pixels.Color(150, 0, 0)); // Moderately bright green color.
  }


  pixels.show(); // This sends the updated pixel color to the hardware.


  if (pressedButton != 0) {
    // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
    // Here, "Button pressed!" is the Payload, but this could be changed to a sensor reading, for example.

    if (client.publish(mqtt_topic1, dtostrf(pressedButton, 2, 0, msgBuffer))) {
      Serial.println("Button pushed and message sent!");
    }
    else {
      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
      client.connect(clientID, mqtt_username, mqtt_password);
    }
  }
  client.publish(mqtt_topic2, dtostrf(faderValue1, 4, 0, msgBuffer));
  client.publish(mqtt_topic3, dtostrf(faderValue2, 4, 0, msgBuffer));
  client.publish(mqtt_topic4, dtostrf(faderValue3, 2, 0, msgBuffer));

  Serial.println("Sw1:" + String(faderValue1));
  Serial.println("Sw2:" + String(faderValue2));
  Serial.println("Sw3:" + String(faderValue3));
  Serial.println(pressedButton);

  delay(50);
}

