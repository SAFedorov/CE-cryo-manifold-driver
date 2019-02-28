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