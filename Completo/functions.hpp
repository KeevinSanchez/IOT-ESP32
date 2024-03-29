
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "credentials.hpp"

WiFiClient wificon;
PubSubClient client(wificon);

DHT_Unified dht(DHTPIN, DHTTYPE);

// Función de reconeción 
void reconnet(){
    while(!client.connected()){
        Serial.println("Conectando al Broker MQTT");
        if(client.connect(ADAFRUIT_ID, ADAFRUIT_USER, ADAFRUIT_KEY)){
            Serial.println("MQTT conectado!");
            client.subscribe(ADAFRUIT_FEED_ON_OFF);
            Serial.println("Suscrito al tópico: " + String(ADAFRUIT_FEED_ON_OFF));
        }else{
            Serial.print("Falló, rc= ");
            Serial.println(client.state());
            vTaskDelay(2000);
        }
    }
}
// Función para Publicar por MQTT
void mqtt_publish(String feed, int val){
    String value = String(val);
    if(client.connected()){
        client.publish(feed.c_str(), value.c_str());
        Serial.println("Publicando al tópico: " + String(feed) + " | mensaje: " + value);        
    }
}
// Función para capturar data por MQTT
void callback(char *topic, byte *payload, unsigned int length){
    
    String mensaje  = "";
    String str_topic(topic); 

    for (uint16_t i = 0; i < length; i++){
        mensaje += (char)payload[i];
    }

    mensaje.trim();

    if(mensaje == "ON"){
        digitalWrite(LED_1, HIGH);
        mqtt_publish(ADAFRUIT_FEED_LED, digitalRead(LED_1));
    }else{
        digitalWrite(LED_1, LOW);
        mqtt_publish(ADAFRUIT_FEED_LED, digitalRead(LED_1));
    }

    Serial.println("Tópico: " + str_topic);
    Serial.println("Mensaje: " + mensaje); 

}
// Capturar el ChipID para Id de MQTT
void get_MQTT_ID(){
    uint64_t chipid = ESP.getEfuseMac();    
    snprintf(ADAFRUIT_ID, sizeof(ADAFRUIT_ID),"%llu", chipid );
}
// Gestionar la conexión MQTT
void WiFi_begin(){
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    vTaskDelay(500);
    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");        
        vTaskDelay(500);
    }
    Serial.println("WiFi conectado al IP: ");
    Serial.println(WiFi.localIP());     
}
// Loop para MQTT
void loopMQTT(){
    if(!client.connected()){
        reconnet();
    }
    client.loop();
}
// LED
void set_LED(){
    pinMode(LED_1, OUTPUT);
}