/*
Controller 1: 6 Switches, 1 Button and a LED

TODO:
=Switch button and switch4 so D8 wont have power on startup
=Update leds on startup
=send update to broker every x time
=send update to broker at startup
=Change sending button change to changing possible multiple buttons at once (moving it to the for loop)
*/
#include <Bounce2.h> // Used for "debouncing" the pushbutton
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <Adafruit_NeoPixel.h> //Lib for the programable leds




//Pin out for Wemos Mini D1(R2 has the same)
const int LEDPIN = 0;       // Connect LED (PL9823) to pin D3
const int SWITCHPIN1 = 5;   // Connect switch 1 to pin D1
const int SWITCHPIN2 = 4;   // Connect switch 2 to pin D2
const int SWITCHPIN3 = 16;  // Connect switch 3 to pin D0
const int SWITCHPIN4 = 15;  // Connect switch 4 to pin D8
const int SWITCHPIN5 = 14;  // Connect switch 5 to pin D5
const int SWITCHPIN6 = 12;  // Connect switch 6 to pin D6
const int BUTTONPIN = 13;  // Connect switch 7 to pin D7

const byte NUMPIXELS = 6;    //Amount of LEDS

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_RGB + NEO_KHZ800);

// WiFi
// Make sure to update this for your own WiFi network!
const char* SS_ID = "test";
const char* WIFI_PASSWORD = "testtest";
const char* TEST_SS_ID = "Ziggo78F5D45";
const char* TEST_WIFI_PASSWORD = "Sx7phx8fnkeP";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* MQTT_SERVER = "192.168.178.194";
const char* MQTT_TOPIC_BUTTON = "c1_b1";
const char* MQTT_USERNAME = "uncloud";
const char* MQTT_PASSWORD = "uncloud";
const char* CLIENT_ID = "controller1"; // The client id identifies the ESP8266 device. Think of it a bit like a hostname.

char msgBuffer[20];

//// Initialise the Bouncer objects
Bounce BUTTON[7] = {Bounce(), Bounce(), Bounce(), Bounce(), Bounce(), Bounce(), Bounce()};

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

  pinMode(SWITCHPIN1, INPUT);
  pinMode(SWITCHPIN2, INPUT);
  pinMode(SWITCHPIN3, INPUT);
  pinMode(SWITCHPIN4, INPUT);
  pinMode(SWITCHPIN5, INPUT);
  pinMode(SWITCHPIN6, INPUT);
  pinMode(BUTTONPIN, INPUT);

  // Setup pushbutton Bouncer object
  BUTTON[0].attach(SWITCHPIN1);
  BUTTON[1].attach(SWITCHPIN2);
  BUTTON[2].attach(SWITCHPIN3);
  BUTTON[3].attach(SWITCHPIN4);
  BUTTON[4].attach(SWITCHPIN5);
  BUTTON[5].attach(SWITCHPIN6);
  BUTTON[6].attach(BUTTONPIN);

  for (int i = 0; i < 7; i++) {
    BUTTON[i].interval(5);
  }

  pixelsOneColor(100,155,000);
  //Wait a minute before starting up so we are sure the Raspberry has started (Cancel by pressing button)
  while((digitalRead(BUTTONPIN) == LOW) && (millis() < 60000)){ 
   delay(50);
  }

  delay(500);
  
  //WIFI INIT
  Serial.print("Connecting to ");
  Serial.println(SS_ID);
  
  // Connect to the WiFi (if button is being hold, use test wifi)
  if (digitalRead(BUTTONPIN) == LOW) WiFi.begin(SS_ID, WIFI_PASSWORD);
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

  for (int i = 0; i < 7; i++) {
    BUTTON[i].update();//Update the button state
    if (BUTTON[i].rose()) { //If it went from low to high
      pressedButton = i + 1; 
      if (i != 6) pixels.setPixelColor(5 - i, pixels.Color(0, 0, 0)); //turn of the LED
    }
    if (BUTTON[i].fell()) { //if it went from high to low
      pressedButton = i + 8;
      if (i != 6) pixels.setPixelColor(5 - i, pixels.Color(155, 0, 0)); //turn the LED red
    }
  }
  
  pixels.show();
  
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
  
//  Uncomment these lines for debugging
//  Serial.println("Sw1:" + String(digitalRead(SWITCHPIN1)));
//  Serial.println("Sw2:" + String(digitalRead(SWITCHPIN2)));
//  Serial.println("Sw3:" + String(digitalRead(SWITCHPIN3)));
//  Serial.println("Sw4:" + String(digitalRead(SWITCHPIN4)));
//  Serial.println("Sw5:" + String(digitalRead(SWITCHPIN5)));
//  Serial.println("Sw6:" + String(digitalRead(SWITCHPIN6)));
//  Serial.println("BT:" + String(digitalRead(BUTTONPIN)));

  delay(5);
}

