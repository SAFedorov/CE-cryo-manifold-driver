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

/* Serial port configuration */
#define COM_TERMINATOR  '\n'
#define COM_BAUD_RATE   9600
#define COM_BUFF_SIZE   256
#define COM_TIMEOUT     50 // ms



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
  register_button(&btn_list, 45, &toggle_valve1);       // valve 1 open/close
  register_button(&btn_list, 49, &toggle_valve23);      // valve 2 and 3 open/close
  register_button(&btn_list, 17, &toggle_valve4);       // valve 4 open/close
  register_button(&btn_list, 23, &toggle_valve5);       // valve 5 open/close
  register_button(&btn_list, 48, &toggle_valve7);       // valve 7 open/close
  
  register_button(&btn_list, 47, &toggle_recirculator); // recirculator on/off
  register_button(&btn_list, 46, &toggle_cryocooler);   // cryocooler on/off

  register_button(&btn_list, 6, &set_all_off);    // all off

  /* Start serial communication */
  Serial1.setTimeout(COM_TIMEOUT);
  Serial1.begin(COM_BAUD_RATE);
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

/*
 * Toggle output pin state between HIGH and LOW
 */
void toggle_pin(int pin)
{
  if(digitalRead(pin))
  {
    digitalWrite(pin, LOW);
  }
  else
  {
    digitalWrite(pin, HIGH);
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
 * Toggle valves
 */
void toggle_valve1(struct ce_button* btn){
  toggle_pin(VALVE1_PIN);
}

void toggle_valve23(struct ce_button* btn){
  toggle_pin(VALVE2_PIN);
  toggle_pin(VALVE3_PIN);
}

void toggle_valve4(struct ce_button* btn){
  toggle_pin(VALVE4_PIN);
}

void toggle_valve5(struct ce_button* btn){
  toggle_pin(VALVE5_PIN);
}

void toggle_valve7(struct ce_button* btn){
  toggle_pin(VALVE7_PIN);
}

void toggle_recirculator(struct ce_button* btn){
  toggle_pin(RECIRC_PIN);
}

void toggle_cryocooler(struct ce_button* btn){
  toggle_pin(COOLER_PIN);
}
