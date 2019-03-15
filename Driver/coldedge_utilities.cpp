#include <Arduino.h>
#include <LiquidCrystal_I2C.h> // newliquidcrystal 
#include "coldedge_utilities.h"


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


double read_pressure()
{
  double p;
  p = analogRead(PRESS_PIN) / 1024.0; //get value relative to the full range
  p = p * 200;  //convert to psi
  return p;
}


void set_all_off()
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


void display_pressure(LiquidCrystal_I2C* lcd)
{
	double p;
	char str[20];
	
	p = read_pressure();	

	lcd->setCursor(1, 1);	
	lcd->print("Supply pressure: ");

	lcd->setCursor(1, 2);
	lcd->print(p);

	/* Extra spaces at the end are needed to overwrite remaining symbols 
	 * in the case if the new string is shorter than the old one 
	 */
	lcd->print(" PSI      "); 
}


void display_startup(LiquidCrystal_I2C* lcd)
{
	lcd->setCursor(3, 1); 
	lcd->print("Starting..."); 
}