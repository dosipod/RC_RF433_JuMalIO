#pragma once

#include "wled.h"

#include <RCSwitch.h>

#ifndef USERMOD_RC_SWITCH_PIN
#ifdef ARDUINO_ARCH_ESP32
#define USERMOD_RC_SWITCH_PIN 22
#else //ESP8266 boards
#define USERMOD_RC_SWITCH_PIN 4
#endif
#endif

#ifndef USERMOD_RC_SWITCH_PULSE_LENGTH
#define USERMOD_RC_SWITCH_PULSE_LENGTH 50
#endif

#ifndef USERMOD_RC_SWITCH_PROTOCOL
#define USERMOD_RC_SWITCH_PROTOCOL 15
#endif

RCSwitch mySwitch = RCSwitch();

class RcSwitchV2 : public Usermod {

  private:

    bool initDone = false;
    static const char _name[];
    #ifndef WLED_DISABLE_MQTT
    static const char _topic[];
    #endif
    static const char _payload[];

  public:

    void setup()
    {
      mySwitch.enableTransmit(USERMOD_RC_SWITCH_PIN);
      mySwitch.setPulseLength(USERMOD_RC_SWITCH_PULSE_LENGTH);
      mySwitch.setProtocol(USERMOD_RC_SWITCH_PROTOCOL);

      initDone = true;
    }
    
    void loop() {}

    void readFromJsonState(JsonObject& root)
    {
      if (!initDone) return;  // prevent crash on boot applyPreset()

      JsonObject usermod = root[FPSTR(_name)];
      if (!usermod.isNull())
      {
        if (usermod[FPSTR(_payload)].is<const char*>())
        {
          mySwitch.send(usermod[FPSTR(_payload)].as<const char*>());
        }
      }
    }

#ifndef WLED_DISABLE_MQTT
    bool onMqttMessage(char* topic, char* payload)
    {
      if (strlen(topic) == 9 && strncmp_P(topic, PSTR(_topic), 9) == 0)
      {
        mySwitch.send(payload);

        return true;
      }

      return false;
    }

    void onMqttConnect(bool sessionPresent)
    {
      //(re)subscribe to required topics
      char subuf[64];
      if (mqttDeviceTopic[0] != 0)
      {
        strcpy(subuf, mqttDeviceTopic);
        strcat_P(subuf, PSTR(_topic));
        mqtt->subscribe(subuf, 0);
      }
    }
#endif

    uint16_t getId()
    {
      return USERMOD_ID_RC_SWITCH;
    }
};

const char RcSwitchV2::_name[] PROGMEM = "RcSwitch";
#ifndef WLED_DISABLE_MQTT
const char RcSwitchV2::_topic[] PROGMEM = "/rcswitch";
#endif
const char RcSwitchV2::_payload[] PROGMEM = "payload";
