// https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino

#include <ArduinoOTA.h>
#include "Display.h"

void initialiseArduinoOTA(char * mdns_name, char * ota_password) {
    ArduinoOTA
        .onStart([]() {
            String type;
                if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
                else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start OTA updating " + type);
            display.showMessage("OTA: " + type);
        })
        .onEnd([]() {
            Serial.println("\nEnd");
            display.showMessage("OTA: OK, boot");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("OTA update progress: %u%%\r", (progress / (total / 100)));
            if (progress % 3 == 0) {
                digitalWrite(PIN_ONBOARD_LED, !digitalRead(PIN_ONBOARD_LED));
            }
        })
        .onError([](ota_error_t error) {
            Serial.printf("OTA update error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) {
                Serial.println("Auth Failed");
                display.showMessage("OTA err: Auth");
            }
            else if (error == OTA_BEGIN_ERROR) {
                Serial.println("Begin Failed");
                display.showMessage("OTA err: Begin");
            }
            else if (error == OTA_CONNECT_ERROR) {
                Serial.println("Connect Failed");
                display.showMessage("OTA err: Conn");
            }
            else if (error == OTA_RECEIVE_ERROR) {
                Serial.println("Receive Failed");
                display.showMessage("OTA: Rec fail");
            }
            else if (error == OTA_END_ERROR) {
                Serial.println("End Failed");
                display.showMessage("OTA: End fail");
            }
        });
    
    ArduinoOTA.setHostname(mdns_name);
    ArduinoOTA.setPassword(ota_password);
    ArduinoOTA.begin();
}
