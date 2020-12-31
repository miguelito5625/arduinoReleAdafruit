// Recordatorio, para Relay v1 hay que retirar la R2 del relay y hacer un puente en el relay en los pines 6 y 8 segun disposicion del esp01/01s, tambien el encendido cambia de valor en el relay v1 vs el v4

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#include <AdafruitIO.h>
#include <Adafruit_MQTT.h>
#include <ArduinoHttpClient.h>
#include <PubSubClient.h>


/************************ Adafruit IO Configuration *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME    "exbinario"
#define IO_KEY         "aio_jPVM32gKIIUqTpEVG9j2ygGswvDs"

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, "Repetidor NXY300", "a1a2a3a4a5");

//#define LED_PIN 0
#define OTHER_PIN 0

// set up the 'command' feed
AdafruitIO_Feed *command = io.feed("sala casa mama");

//for LED status
#include <Ticker.h>
Ticker ticker;

#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif

int LED = LED_BUILTIN;

void tick()
{
  //toggle state
  digitalWrite(LED, !digitalRead(LED));     // set pin to the opposite state
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}


void setup() {


  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // put your setup code here, to run once:
  Serial.begin(115200);

  //set led pin as output
  pinMode(LED, OUTPUT);
  pinMode(OTHER_PIN, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - for testing
  // wm.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(LED, LOW);

  command->onMessage(handleMessage);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop() {
  // put your main code here, to run repeatedly:
  io.run();

}





void handleMessage(AdafruitIO_Data *data) {

  int command = data->toInt();

  if (command == 1) { //light up the LED
    Serial.print("received <- ");
    Serial.println(command);
    digitalWrite(OTHER_PIN, HIGH);
  } else {
    Serial.print("received <- ");
    Serial.println(command);
    digitalWrite(OTHER_PIN, LOW);
  }
}
