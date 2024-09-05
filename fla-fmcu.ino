////////////////////////////////////////////////////////////////////////////////
// FMCU Button code
// By Fran Sanchez (The Beach Lab)
// January 2022
//
// IDE:
//   Arduino 2.0.0
// Platform:
//   esp32 2.0.5 - https://github.com/espressif/arduino-esp32
// Board:
//   XIAO_ESP32C3
// Libraries:
//   Adafruit_MQTT
////////////////////////////////////////////////////////////////////////////////

// Includes

#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "config.h"

////////////////////////////////////////////////////////////////////////////////

// Pinouts according to FabXAIO
// Modify to your needs

const int buttonPin = D7; 	// the number of the pushbutton pin GPIO 10 ESP32
const int ledPin    = D6; 	// the number of the LED pin  Internal LED 

////////////////////////////////////////////////////////////////////////////////

int buttonState     = 0;  	// variable for reading the pushbutton status

// MQTT Settings
#define AIO_SERVER      "fmcu.fablabs.io"

// Using port 8883 for MQTTS
#define AIO_SERVERPORT  1883

// MQTT Account Configuration
#define AIO_USERNAME    "fmcu"
#define AIO_KEY         "Push 1 Button"

// WiFiFlientSecure for SSL/TLS support
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed with the id for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish alive = Adafruit_MQTT_Publish(&mqtt, "fmcu/id");

void setup()
{
  Serial.begin(9600);
  delay(100);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

// Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  delay(1000);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(2000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    blinkLED();
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
}

void blinkLED(){
  for(int i = 0; i < 10; i++){
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(50);
  }
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi not connected....");
    blinkLED();
  } else{
    //Serial.println("Yes.");

    ////////////////////////////////////////
    // Publish to MQTT server
    // Ensure the connection to the MQTT server is alive (this will make the first
    // connection and automatically reconnect when disconnected).  See the MQTT_connect
    // function definition further below.
    MQTT_connect();
    buttonState = digitalRead(buttonPin);

    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == HIGH) {
      // Now we can publish stuff!
      Serial.print(F("\nSending id "));
      Serial.print(id);
      Serial.print(F(" to fmcu feed..."));
      if (! alive.publish(id)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
        digitalWrite(ledPin, HIGH);
        delay(500);
      }
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
  
  delay(100);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
  
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    blinkLED();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }

  Serial.println("MQTT Connected!");
}
