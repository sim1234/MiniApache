/*
  ListingDirectory example

 A simple web server that hosts files from SD card.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 27.11.2015
 by Szymon Zmilczak
 */

#include <Ethernet.h>
#include <MiniApache.h>
#include <SD.h>
#include <SPI.h>

byte mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // MAC address from Ethernet shield sticker under board
IPAddress ip(192, 168, 0, 179); // IP address, may need to be changed depending on network
MiniApache apache(80); // Create a server at port 80

void setup(){
    Serial.begin(9600); // Serial for debugging
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH); // Disable Ethernet chip
    Ethernet.begin(mac, ip);  // Initialize Ethernet device
    apache.begin("/served", 4); // Initialize MiniApache server: served path and SD card pin
}

void loop() {
  apache.PendingRequest();
  apache.ProcessRequest();
}



