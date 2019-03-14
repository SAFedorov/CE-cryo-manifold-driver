#include <Arduino.h>
#include <LiquidCrystal_I2C.h> // newliquidcrystal 

/* LCD parameters */
#define I2C_ADDR	0x27  // Define I2C Address where the SainSmart LCD is 3f
#define LCD_WIDTH	20
#define LCD_HIGHT	4

/* LCD pins */
#define BACKLIGHT_PIN   3
#define EN_PIN			2
#define RW_PIN			1
#define RS_PIN			0
#define D4_PIN			4
#define D5_PIN			5
#define D6_PIN			6
#define D7_PIN			7

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


/*
 * Display pressure in PSI on the screen
 */
void display_pressure(LiquidCrystal_I2C lcd);


/*
 * Display startup screen
 */
void display_startup(LiquidCrystal_I2C lcd);

