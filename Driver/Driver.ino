#include <Arduino.h>

/* Setup buttons:  {pin, previous value, toggle status, set flag} */
int but1p49[4] = {49, 0, 0, 0}; // Pin 49 evacuate mode - PL0/35
int but2p48[4] = {48, 0, 0, 0}; // Pin 48 purge mode - PL1/36
int but3p47[4] = {47, 0, 0, 0}; // Pin 47 start mode - PL2/37
int but4p46[4] = {46, 0, 0, 0}; // Pin 46 stop cooling - PL3/38
int but5p45[4] = {45, 0, 0, 0}; // Pin 45 autocool system - PL4/39
int butRp44[4] = {44, 0, 0, 0}; // Pin 44 reset EEPROM - PL5/40
int butSp23[4] = {23, 0, 0, 0}; // Pin 23 toggle recirculator - PA1/77
int but1p50[4] = {17, 0, 0, 0}; // Pin 50 cancel - PH0/12  JCP
int butEp51[4] = {16, 0, 0, 0}; // Pin 51 encoder button - PH1/13 JCP
int but2p52[4] = {6, 0, 0, 0};  // Pin 52 toggle arduino/lv mode - PH2/14 JCP
int but3p53[4] = {5, 0, 0, 0};  // Pin 53 charge - PH3/15   JCP

/* Define pins */
int v1 = 32;  // Valve 1 RC_OUT6 PC5/58
int v2 = 33;  // Valve 2 RC_OUT5 PC4/57
int v3 = 34;  // Valve 3 RC_OUT4 PC3/56
int v4 = 35;  // Valve 4 RC_OUT3 PC2/55
int v5 = 36;  // Valve 5 RC_OUT2 PC1/54
int v6 = 37;  // Valve 6 RC_OUT1 PC0/53
int v7 = 38;  // Valve 7 RC_OUT9 PD7/50 Helium charge
int Rr = 28;  // Recirculator PC7/60  JCP
int Rc = 29;  // He compressor PC6/59 JCP
int Vs1 = 30; // PA6/72 relay bank 3 K3 dsub 12
int Vs2 = 31; // PA7/71 relay bank 3 K4 dsub 13

int Pp = 0;   // analog pin for reading pressure ADC0/78 

int vM = 39;  // Arduino/LV mode status LED PG2/70 
int vC = 40;  // charge status LED PG1/52

void setup() {
  
  analogReference(DEFAULT); // Top of the analog input range is 5V

  /* Configure push buttons */
  pinMode(butSp23[0], INPUT_PULLUP);
  pinMode(but1p49[0], INPUT_PULLUP);
  pinMode(but2p48[0], INPUT_PULLUP);
  pinMode(but3p47[0], INPUT_PULLUP);
  pinMode(but4p46[0], INPUT_PULLUP);
  pinMode(but5p45[0], INPUT_PULLUP);
  pinMode(but1p50[0], INPUT_PULLUP);
  pinMode(butEp51[0], INPUT_PULLUP);
  pinMode(but2p52[0], INPUT_PULLUP);
  pinMode(but3p53[0], INPUT_PULLUP);
  pinMode(butRp44[0], INPUT_PULLUP);

  /* Configure LEDs */
  pinMode(v1, OUTPUT);
  pinMode(v2, OUTPUT);
  pinMode(v3, OUTPUT);
  pinMode(v4, OUTPUT);
  pinMode(v5, OUTPUT);
  pinMode(v6, OUTPUT);
  pinMode(v7, OUTPUT);
  pinMode(vM, OUTPUT);
  pinMode(vC, OUTPUT);
  pinMode(Rr, OUTPUT);
  pinMode(Rc, OUTPUT);

  /* Set LEDs off */
  digitalWrite(v1, LOW);  //Helium Supply
  digitalWrite(v2, LOW);  //Charge Supply
  digitalWrite(v3, LOW);  //Charge Return
  digitalWrite(v4, LOW);  //Helium Return
  digitalWrite(v5, LOW);  //Vacuum
  digitalWrite(v6, LOW);  //Sample well
  digitalWrite(vM, LOW);  //arduino/LV mode LED
  digitalWrite(vC, LOW);  //helium charge LED
  digitalWrite(Rr, LOW);  //recirculator
  digitalWrite(Rc, LOW);  //compressor
}


/*
 * Main function that is executed continuously
 */
void loop() {
  while(1)
  {
    senseButtons();
    //processComm();
  }
}


void senseButtons()
{
  bool Bpsh = false;
  Bpsh = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
  if(Bpsh == true)
  {
    
  }
}

/*
 * Check if button was pressed
 */
 
bool butDebounce(int pinNum, int &Lst, int &Bst , int &Bset) {

  int readPin;

  readPin = digitalRead(pinNum);
  
  // Button state is changed
  if(readPin != Lst)
  {   
    // Change was true to false, so button was pressed
    if(readPin == false){
      Bset=0; // put button into set mode, button only arms when button is released
      Lst = readPin; //  save pin state
      return false;
    }
    // Change was false to true, so button was released
    if(readPin == true){
      Bset=1; //put button into set mode. button only arms when button is released
      Bst = !Bst;   //toggle button status      
      Lst = readPin; //  save pin state
      return true;
    }
  }

  // Button state did not change
  return false;
}
