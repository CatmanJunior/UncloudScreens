/*
   ESP8266 (Adafruit HUZZAH) Mosquitto MQTT Publish Example
   Thomas Varnish (https://github.com/tvarnish), (https://www.instructables.com/member/Tango172)
   Made as part of my MQTT Instructable - "How to use MQTT with the Raspberry Pi and ESP8266"
*/
#include <Bounce2.h> // Used for "debouncing" the pushbutton
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker

const int ledPin = 0; // This code uses the built-in led for visual feedback that the button has been pressed
const int buttonPin1 = 16; // Connect your button to pin #13
const int buttonPin2 = 4; // Connect your button to pin #13
const int buttonPin3 = 0; // Connect your button to pin #13
const int buttonPin4 = 2; // Connect your button to pin #13
const int buttonPin5 = 14; // Connect your button to pin #13
const int buttonPin6 = 12; // Connect your button to pin #13
const int buttonPin7 = 13; // Connect your button to pin #13
// WiFi
// Make sure to update this for your own WiFi network!
const char* ssid = "Ziggo78F5D45";
const char* wifi_password = "Sx7phx8fnkeP";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "192.168.178.194";
const char* mqtt_topic = "button";
const char* mqtt_username = "uncloud";
const char* mqtt_password = "uncloud";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "controller1";

// Initialise the Pushbutton Bouncer object
Bounce but1 = Bounce();
Bounce but2 = Bounce();
Bounce but3 = Bounce();
Bounce but4 = Bounce();
Bounce but5 = Bounce();
Bounce but6 = Bounce();
Bounce but7 = Bounce();
// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);
  pinMode(buttonPin5, INPUT);
  pinMode(buttonPin6, INPUT);
  pinMode(buttonPin7, INPUT);
  // Switch the on-board LED off to start with
  digitalWrite(ledPin, HIGH);

  // Setup pushbutton Bouncer object
  but1.attach(buttonPin1);
  but1.interval(5);
  but2.attach(buttonPin2);
  but2.interval(5);
  but3.attach(buttonPin3);
  but3.interval(5);
  but4.attach(buttonPin4);
  but4.interval(5);
  but5.attach(buttonPin5);
  but5.interval(5);
  but6.attach(buttonPin6);
  but6.interval(5);
  but7.attach(buttonPin7);
  but7.interval(5);
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
  int butpress = 0;
  // Update button state
  // This needs to be called so that the Bouncer object can check if the button has been pressed
  but1.update();
  but2.update();
  but3.update();
  but4.update();
  but5.update();
  but6.update();
  but7.update();

  if (but1.rose()) {
    butpress = 1;
  }
  if (but2.rose()) {
    butpress = 2;
  }
  if (but3.rose()) {
    butpress = 3;
  }
  if (but4.rose()) {
    butpress = 4;
  }
  if (but5.rose()) {
    butpress = 5;
  }
  if (but6.rose()) {
    butpress = 6;
  }
  if (but7.rose()) {
    butpress = 7;
  }

  if (butpress != 0) {
    // PUBLISH to the MQTT Broker (topic = mqtt_topic, defined at the beginning)
    // Here, "Button pressed!" is the Payload, but this could be changed to a sensor reading, for example.
    if (butpress == 1) {

      if (client.publish(mqtt_topic, "1")) {
        Serial.println("Button pushed and message sent!");
      }
      else {
        Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
        client.connect(clientID, mqtt_username, mqtt_password);
      }
    }
    if (butpress == 2) {

      if (client.publish(mqtt_topic, "2")) {
        Serial.println("Button pushed and message sent!");
      }
      else {
        Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
        client.connect(clientID, mqtt_username, mqtt_password);
      }
    }
    if (butpress == 3) {

      if (client.publish(mqtt_topic, "3")) {
        Serial.println("Button pushed and message sent!");
      }
      else {
        Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
        client.connect(clientID, mqtt_username, mqtt_password);
      }
    }
    if (butpress == 4) {

      if (client.publish(mqtt_topic, "4")) {
        Serial.println("Button pushed and message sent!");
      }
      else {
        Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
        client.connect(clientID, mqtt_username, mqtt_password);
      }
    }
    if (butpress == 5) {

      if (client.publish(mqtt_topic, "5")) {
        Serial.println("Button pushed and message sent!");
      }
      else {
        Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
        client.connect(clientID, mqtt_username, mqtt_password);
      }
    }
    if (butpress == 6) {

      if (client.publish(mqtt_topic, "6")) {
        Serial.println("Button pushed and message sent!");
      }
      else {
        Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
        client.connect(clientID, mqtt_username, mqtt_password);
      }
    }
    if (butpress == 7) {

      if (client.publish(mqtt_topic, "7")) {
        Serial.println("Button pushed and message sent!");
      }
      else {
        Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
        client.connect(clientID, mqtt_username, mqtt_password);
      }
    }
    Serial.println(butpress);
    // Again, client.publish will return a boolean value depending on whether it succeded or not.
    // If the message failed to send, we will try again, as the connection may have broken.
    //    else {
    //      Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    //      client.connect(clientID, mqtt_username, mqtt_password);
    //    }
//
//    delay(10);//
//    publish(mqtt_topic, "0");
  }
}
