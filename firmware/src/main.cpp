#include "Arduino.h"
#include "isp.h"
#include "programs.h"

#define LED_GREEN 7
#define LED_RED   6

void setup() {
  Serial.begin(9600);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  isp_init();
}

void loop(void) {
  start_pmode();
  digitalWrite(LED_RED, 1);
  delay(100);
  digitalWrite(LED_RED, 0);
  if(isp_read_signature() == 0x1e9502)
  {
    program_fuses(0xff,0xde);
    end_pmode();
    digitalWrite(LED_RED, 1);
    delay(100);
    digitalWrite(LED_RED, 0);
    start_pmode();
    program_to_flash(bootloader_bin, bootloader_bin_len);
    digitalWrite(LED_GREEN, 1);
  }
  else
  {
    digitalWrite(LED_RED, 1);
  }
  end_pmode();
  
  while(1);
}