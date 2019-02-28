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


/*
 * Toggle output pin state between HIGH and LOW
 */
void toggle_pin(int pin);


/*
 * Read supply pressure in PSI
 */
double read_pressure();


/*
 * Set all outputs to 'off' state
 */
void set_all_off();