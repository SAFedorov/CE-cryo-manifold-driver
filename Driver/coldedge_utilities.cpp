#include <Arduino.h>
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
  p = analogRead(PRESS_PIN) / 1024.0; //get value relative to full range
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