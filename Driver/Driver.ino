#include "buttons.h"
#include <Arduino.h>


/* Valve pins */
#define VALVE1_PIN  32
#define VALVE2_PIN  33
#define VALVE3_PIN  34
#define VALVE4_PIN  35
#define VALVE5_PIN  36
// VALVE6_PIN - 37, valve 6 is not used 
#define VALVE7_PIN  38

/* LED pins */
#define LED1_PIN    39
#define LED2_PIN    40

/* Other pins */
#define RECIRC_PIN  28
#define COOLER_PIN  29
#define PRESS_PIN   0   // analog pin for reading pressure ADC0/78

/* Global variables */
struct ce_button_list btn_list;

void setup() {
  
  analogReference(DEFAULT); // Set the top of the analog input range (5V)

  pinMode(VALVE1_PIN, OUTPUT);
  pinMode(VALVE2_PIN, OUTPUT);
  pinMode(VALVE3_PIN, OUTPUT);
  pinMode(VALVE4_PIN, OUTPUT);
  pinMode(VALVE5_PIN, OUTPUT);
  pinMode(VALVE7_PIN, OUTPUT);
  pinMode(RECIRC_PIN, OUTPUT);
  pinMode(COOLER_PIN, OUTPUT);

  set_all_off();

  btn_list.head = NULL;
  register_button(&btn_list, 45, &toggle_valve1);   // valve 1 open/close
  register_button(&btn_list, 49, &toggle_valve1);   // valve 2 open/close
  register_button(&btn_list, 48, &toggle_valve1);   // valve 3 open/close
  register_button(&btn_list, 44, &toggle_valve1);   // valve 4 open/close
  register_button(&btn_list, 23, NULL);   // valve 5 open/close
  register_button(&btn_list, 5, NULL);    // valve 7 open/close
  
  register_button(&btn_list, 47, NULL);    // recirculator on/off
  register_button(&btn_list, 46, NULL);    // cryocooler on/off

  register_button(&btn_list, 6, &set_all_off);    // all off
}


/*
 * Main function that is executed continuously
 */
void loop() {
  while(1)
  {
    execute_buttons(&btn_list);
    //processComm();
  }
}

/* Callbacks */

/*
 * Set all outputs to 'off' state
 */
void
set_all_off()
{
  digitalWrite(VALVE1_PIN, LOW);  
  digitalWrite(VALVE2_PIN, LOW);  
  digitalWrite(VALVE3_PIN, LOW);  
  digitalWrite(VALVE4_PIN, LOW);  
  digitalWrite(VALVE5_PIN, LOW);  
  digitalWrite(VALVE7_PIN, LOW);  
  digitalWrite(RECIRC_PIN, LOW);  
  digitalWrite(COOLER_PIN, LOW);  
}

/*
 * Switch valve 1
 */
void toggle_valve1(struct ce_button*)
{
  if(digitalRead(VALVE1_PIN) == HIGH)
  {
    digitalWrite(VALVE1_PIN, LOW);
  }
  else
  {
    digitalWrite(VALVE1_PIN, HIGH);
  }
}
