#include <Arduino.h>
#include <USBComposite.h>

// Macros
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#define HOLDOFF 100

// Global Configuration:
int SWITCH_PINS[10] =  {PB12, PB13, PB15, PA15, PA10, PA8, PB14, PB4, PA9, PB3};   // Footswitch pins
bool SWITCH_STATUS[10];
unsigned long SWITCH_TIME[10];

USBMIDI MIDI;

void setup() {
  afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);   // Disable JTAG Pins

  for(uint8_t i = 0; i<NELEMS(SWITCH_PINS); i++) {
    pinMode(SWITCH_PINS[i], INPUT_PULLDOWN);
  }

  USBComposite.clear();   // Setup USB Device Properties
  USBComposite.setProductId(0x0031);
  USBComposite.setManufacturerString("BOJIT");
  USBComposite.setProductString("BOJIT Footswitch");

  MIDI.registerComponent();   // Register USB Endpoints
  USBComposite.begin();
  while (!USBComposite); // Wait for USB Connection to be Established
}

void loop() {
  unsigned long time = millis();
  for(uint8_t i = 0; i<NELEMS(SWITCH_PINS); i++) {
    bool switch_status = digitalRead(SWITCH_PINS[i]);
    if((switch_status != SWITCH_STATUS[i]) && (time - SWITCH_TIME[i] > HOLDOFF)) {
      if(switch_status == true) {
        MIDI.sendControlChange(4, i + 1, 127);
      }
      if(switch_status == false) {
        MIDI.sendControlChange(4, i + 1, 0);
      }
      SWITCH_STATUS[i] = switch_status;
      SWITCH_TIME[i] = time;
    }
  }
}