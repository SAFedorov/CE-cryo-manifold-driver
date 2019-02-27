/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

#include <Wire.h>
#include <EEPROM.h>
//#include <scpiparser.h>
//#include <LCD.h>
#include <LiquidCrystal_I2C.h>
//#include <LiquidCrystal.h>

//Beginning of Auto generated function prototypes by Atmel Studio
void startDisplay();
void coldedgeDisplay();
void valveDisplay(String SendSystemStatus);
void buttonSensing();
boolean butDebounce(int pinNum, int& Lst, int& Bst, int& Bset);
void valvetoggle(int vnum, bool state);
void toggleRecirculator();
void closeAllValves();
void m1evacMan();
void m1evacAuto();
void m2purgeMan();
void m2purgeAuto();
void m3startCool();
void m3stopCool();
void m1evacManIndefinite();
void elapsedtimeDisplay(long etime);
void pressureDisplay();
void timeDisplay();
void tAdjust();
void EncoderState(int& rotate);
int ReadEncoder();
static void CheckLVtoggle(String SendRoutine);
void chargeDisplay();
void AutoCharge();
//End of Auto generated function prototypes by Atmel Studio

//setup buttons  pin, previous value, toggle status, set flag
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

int pinEncoderA = 26;  // PA4/74
int pinEncoderB = 27;  // PA5/73

//setup relay pins
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
//int vP = 54;  // Purge status LED PK0/54
//int vE = 55;  // Evacuate status LED PK1/55

long t1Default = 10;  //Default timing 1 (evacuation time) (minutes)
long t2Default = 15;  //Default timing 2  (purge time) (minutes)
long aPressureDefault = 5;  //Default pressure setting for auto mode
long aWarnDefault = 1;  //Default setting for Warning user 1 = yes warn, 0 = no warn
String ChargeModeDefault = "Man"; //Default charging mode "Man" is manual, "Auto" is automatic
long valvedelay = 60000; //delay (in ms) between opening He return and supply
long NNcharge = 100; //how many cycles between charge updates on main screen. 100 cycles updates approx every 15 secs.
long PSI = 0;   //pressure reading
float vlt = 0;  //pressure transducer voltage
long IIcharge = 0;  //index for charge
long tChargeUpdate = 250; //time between refreshing pressure display (milliseconds)

long t1, t2, aPressure, aWarn;

String ChargeMode;

long dtc = 1000; //delta timing for event timer on LCD display
//long dte - not needed
long dt = 50;  //delta timing for setting times
long LDT = 0;   //global variable for last delay time for button debounce, initialization not important
//long DebDel - not needed
long EncoderDel = 10; //debounce delay for encoder
String Cmode = "mAR"; //Arduino Mode
String SerBuf = "";
String SerStr = "";
char inChar = ' ';
String SystemStatus = "Idle";

bool brk = false;

//setup display
#define I2C_ADDR    0x27  // Define I2C Address where the SainSmart LCD is 3f
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

// This defines the I2C configuration for the LCD
LiquidCrystal_I2C  lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

byte CharON[9] = {B00000, B00000, B01111, B01111, B01111, B01111, B00000, B00000}; // Square
byte CharOF[9] = {B00000, B00000, B01111, B01001, B01001, B01111, B00000, B00000}; // Open box
byte CharLE[9] = {B10000, B10000, B10000, B10000, B10000, B10000, B10000, B00000}; // Left edge
byte CharRI[9] = {B00001, B00001, B00001, B00001, B00001, B00001, B00001, B00000}; // Right edge
byte CharRE[9] = {B11111, B10001, B10101, B10101, B10011, B10101, B10101, B11111}; // R for Remote mode
byte CharLO[9] = {B11111, B10111, B10111, B10111, B10111, B10111, B10001, B11111}; // L for Arduino Mode
byte CharSE[9] = {B00000, B01000, B01100, B01110, B01100, B01000, B00000, B00000}; // Right arrowhead
byte CharFI[9] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}; // All black

//--------------------------------------------
//
//  This is the Arduino setup() routine
//
//  Make a single pass through this to initialize all values
//
//--------------------------------------------
void setup() {

  analogReference(DEFAULT);

  //get variables from EEPROM
  t1 = EEPROM.read(0);
  t2 = EEPROM.read(1);
  aPressure = EEPROM.read(2);
  int ChargeModeVal = EEPROM.read(3);
  aWarn = EEPROM.read(4);

  if (ChargeModeVal == 0) {
    ChargeMode = "Man"; //Manual charge
  } else {
    ChargeMode = "Auto"; //Automatic Charge using feedback from pressure sensor
  }

  // I2c is on Arduino 20 SDA/21 SCL ( ATMega 44/43)
  // That is the only place it can be so no configuration is required

  // SPI on Arduino pins 19 RX1/18 TX1 (ATMega 45/46)
  Serial1.begin(9600);
  Serial.begin(9600);

  // Not sure what this is
//  pinMode(20, INPUT);         // Display Data
//  pinMode(19, OUTPUT);        // TXD5V
//  pinMode(20, INPUT_PULLUP);  // Display Data

  // Configure push buttons
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

  pinMode(pinEncoderA, INPUT_PULLUP);     //configure encoder pins
  pinMode(pinEncoderB, INPUT_PULLUP);


  // Configure LEDs
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
//  pinMode(vP, OUTPUT);
//  pinMode(vE, OUTPUT);

  //Set LEDs off
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
//  digitalWrite(vP, LOW);  //Purge LED
//  digitalWrite(vE, LOW);  //Evacuate LED

  // Read initial value of the push buttons
  butSp23[1] = digitalRead(butSp23[0]);
  but1p49[1] = digitalRead(but1p49[0]);
  but2p48[1] = digitalRead(but2p48[0]);
  but3p47[1] = digitalRead(but3p47[0]);
  but4p46[1] = digitalRead(but4p46[0]);
  but5p45[1] = digitalRead(but5p45[0]);
  but1p50[1] = digitalRead(but1p50[0]);
  butEp51[1] = digitalRead(butEp51[0]);
  but2p52[1] = digitalRead(but2p52[0]);
  but3p53[1] = digitalRead(but3p53[0]);
  butRp44[1] = digitalRead(butRp44[0]);

  // Initialize the LCD on the I2C port
  lcd.begin(20, 4);   // Set width and height
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE); // Set backlight
  lcd.setBacklight(HIGH);

  // Create special characters
  lcd.createChar(1, CharOF);
  lcd.createChar(2, CharON);
  lcd.createChar(3, CharRI);
  lcd.createChar(4, CharLE);
  lcd.createChar(5, CharRE);  // Remote mode
  lcd.createChar(6, CharLO);  // Local mode
  lcd.createChar(7, CharSE);  
  lcd.createChar(8, CharFI);

  //bool Bpsh = false;
  startDisplay();
  coldedgeDisplay();
  valveDisplay(SystemStatus);

} //end void setup ------------------------------------------------

// Routines used in setup - startDisplay, coldedgeDisplay, valveDisplay

//--------------------------------------------
//
// Display the ColdEdge banner screen
//
// Display the banner and version then march black boxes across the screen and back
//
//--------------------------------------------
void startDisplay() {

  lcd.setCursor(1, 0); lcd.print("Cooled by ColdEdge");
  lcd.setCursor(3, 1); lcd.print("AutoCOOL  v1.35");  // <<<<<<<<<<<<<<<<<  Version Number for Display
  lcd.setCursor(4, 2); lcd.print("initializing");

  lcd.setCursor(0, 3);
  // Write all black boxes
  for (int i = 0; i < 20; i++) {
    lcd.write(8);
    delay(20);
  }
  lcd.setCursor(0, 3);
  // Clear the boxes
  for (int i = 19; i >= 0; i--) {
    lcd.setCursor(i, 3);
    lcd.print(" ");
    delay(20);
  }
}

//--------------------------------------------
//
// Indicate Local or Remote mode on display
//
// Write the special "L" or "R" in the top right corner of the display
//
//--------------------------------------------
void coldedgeDisplay() {
  lcd.clear();
  lcd.home();
  // Write "L" or "R"
  if (Cmode == "mAR") {
    lcd.setCursor(19, 0);   //If in Arduino mode, update mode
    lcd.write(6);
  } else {
    lcd.setCursor(19, 0);   //If in Labview mode, update mode
    lcd.write(5);
  }
}

//--------------------------------------------
//
// Display valve settings on LCD and LabView
//
//--------------------------------------------
void valveDisplay(String SendSystemStatus) {
  lcd.clear();
  coldedgeDisplay();

  // Build the Valve template
  lcd.setCursor(0, 0);  lcd.print("V1   V4");
  lcd.setCursor(0, 1);  lcd.print("V2   V5");
  lcd.setCursor(0, 2);  lcd.print("V3   V7");
  lcd.setCursor(0, 3);  lcd.print("Re   Co");
  lcd.setCursor(3, 0);  lcd.write(3); lcd.write(4);
  lcd.setCursor(3, 1);  lcd.write(3); lcd.write(4);
  lcd.setCursor(3, 2);  lcd.write(3); lcd.write(4);
  lcd.setCursor(3, 3);  lcd.write(3); lcd.write(4);
  lcd.setCursor(9, 0);  lcd.write(4);
  lcd.setCursor(9, 1);  lcd.write(4);
  lcd.setCursor(9, 2);  lcd.write(4);
  lcd.setCursor(9, 3);  lcd.write(4);

  //check valves, update status and display
  int valvestatus[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; //status of valves -,v1,v2,v3,v4,v5,v7,R,C  index zero is nothing

  //valve1
  lcd.setCursor(2, 0);
  valvestatus[1] = digitalRead(v1);
  if (valvestatus[1] == HIGH) {
    lcd.write(2);
  } else {
    lcd.write(1);
  }

  //valve2
  lcd.setCursor(2, 1);
  valvestatus[2] = digitalRead(v2);
  if (valvestatus[2] == HIGH) {
    lcd.write(2);
  } else {
    lcd.write(1);
  }

  //valve3
  lcd.setCursor(2, 2);
  valvestatus[3] = digitalRead(v3);
  if (valvestatus[3] == HIGH) {
    lcd.write(2);
  } else {
    lcd.write(1);
  }

  //valve4
  lcd.setCursor(7, 0);
  valvestatus[4] = digitalRead(v4);
  if (valvestatus[4] == HIGH) {
    lcd.write(2);
  } else {
    lcd.write(1);
  }

  //valve5
  lcd.setCursor(7, 1);
  valvestatus[5] = digitalRead(v5);
  if (valvestatus[5] == HIGH) {
    lcd.write(2);
  } else {
    lcd.write(1);
  }

  //valve6 is not used

  //valve7
  lcd.setCursor(7, 2);
  valvestatus[7] = digitalRead(v7);
  if (valvestatus[7] == HIGH) {
    lcd.write(2);
  } else {
    lcd.write(1);
  }

  //recirculator
  lcd.setCursor(2, 3);
  valvestatus[8] = digitalRead(Rr);
  if (valvestatus[8] == HIGH) {
    lcd.write(2);
  } else {
    lcd.write(1);
  }

  //compressor
  lcd.setCursor(7, 3);
  valvestatus[9] = digitalRead(Rc);
  if (valvestatus[9] == HIGH) {
    lcd.write(2);
  } else {
    lcd.write(1);
  }

  //v1.26 JCP 11/25/18 Remove extra "+"
  // Send status to the remote terminal
  String sendvalvestatus = "VAINI/" + String(valvestatus[1]) + "/" + String(valvestatus[2]) + "/" + String(valvestatus[3]) +
                           "/" + String(valvestatus[4]) + "/" + String(valvestatus[5]) + "/" + String(valvestatus[7]) +
                           "/" + String(valvestatus[8]) + "/" + String(valvestatus[9]) + "/" + SendSystemStatus + "*";
  Serial1.print(sendvalvestatus);

}


// Spin forever in this service loop ------------------------------------------------------------------

void loop() {

  // 150 msec to make one pass of this loop 
  
  SerStr = "";
  inChar = ' ';
  SerStr.reserve(500);

  while (Serial1.available() && inChar != '*') {      //reads commands from Serial (LabVIEW)
    char inChar = (char)Serial1.read();  //reads current character at serial port
    SerBuf += inChar;                   //appends character to the Serial String
    if (inChar == '*') {
// v1.33 12/3/18 JCP   Remove SerStr display      
//      lcd.setCursor(0, 0); lcd.print("      ");
//      lcd.setCursor(0, 0);
      SerStr = SerBuf.substring(SerBuf.length() - 6, SerBuf.length() - 1);
//      lcd.print(SerStr);
      SerBuf = "";
    }
  }  

  delay(50);    //not sure why this is necessary, but will not initiate commands w/o it

  // put your main code here, to run repeatedly:
  buttonSensing();
  
}

// End of service loop --------------------------------------------------------------------------------------

//////////////////////////
//  Routines  ////////////
//////////////////////////

// Start routine ------------------------------------------------------------------
// --------------------------------------------------------------------------------
//
//  Start a very long buttonSensing routine
//
// Check all the buttons and also LabView messages
//
void buttonSensing() {
  bool Bpsh = false;
  String SerStrCheck = "";
  bool LVthere = true;
  char inCharCheck = ' ';
  SerStrCheck.reserve(500);
  String SerBufCheck = "";

  pressureDisplay();

  //
  // Change Labview Mode
  //
  Bpsh = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
  if (Bpsh == true || SerStr == "LAmTO") {
    if (SerStr == "LAmTO") {      //if labview sent command toggle mode
      but2p52[2] = !but2p52[2];   // 0 = Arduino, 1 = LabView
    }
    // Toggled into LabView mode
    if (but2p52[2] == 1) {
      Serial1.print("ALmLV");  // tell Labview to go to labview mode
      digitalWrite(vM, HIGH); //Labview Mode
      delay(500);
      for (int tt = 1; tt <= 120; tt++) { //loop waiting for labview response
        while (Serial1.available() && inCharCheck != '*') {      //reads commands from Serial (LabVIEW)
          char inCharCheck = (char)Serial1.read();  //reads current character at serial port
          SerBufCheck += inCharCheck;                   //appends character to the Serial String
          if (inCharCheck == '*') {
            lcd.setCursor(14, 3); lcd.print("      ");
            lcd.setCursor(14, 3); lcd.print(SerBufCheck);
            SerStrCheck = SerBufCheck.substring(0, SerBufCheck.length() - 1);
            SerBufCheck = "";
          }
        }  //end string read while
        lcd.setCursor(1, 0);
        lcd.print(SerStrCheck);
        if (SerStrCheck == "LVyes") { //if labview responds send set LVthere=true & break
          LVthere = true;
          Cmode = "mLV";
          break;
        } else {
          LVthere = false;
        }
      }//end for loop timing

      // notify user that labview did not connect. and reset the button.
      if (LVthere == false) {
        //v1.26 JCP 11/25/18 Change failed
        but2p52[2] = 0; //  Toggle back to Arduino mode
        digitalWrite(vM, LOW); //arduino Mode
        Cmode = "mAR";      
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("ERROR|********|ERROR");
        lcd.setCursor(0, 1); lcd.print("  Labview  timeout");
        lcd.setCursor(0, 2); lcd.print("  Check connection");
        lcd.setCursor(0, 3); lcd.print("    Run  Labview");
        delay(5000);
      }

    } else {  // Toggled into Arduino mode
      digitalWrite(vM, LOW);  // Arduino Mode
      but2p52[2] = 0;         //  "
      Cmode = "mAR";          //  "
      Serial1.print("ALmAR");    //tell Labview to go to arduino mode
    }
    lcd.setCursor(14, 3);   // Set "L" or "R"
    lcd.print(Cmode);
    coldedgeDisplay();
    valveDisplay(SystemStatus);
  }// end Change LabView mode


  //
  //Helium charge
  //
  // If manual: Arduino mode - Only when pressed
  //            LabView mode - CHon and CHof
  // Only sense the state of the pushbutton or LAcCHon/of
  // 
  // v1.31 12/1/18 JCP
  //
  // He Charge manual arduino mode or labview mode
  delay(100);
  if (ChargeMode == "Man"){
    //Arduino mode - Check for button pressed or released
    if (Cmode == "mAR") {
      Bpsh = butDebounce(but3p53[0], but3p53[1], but3p53[2], but3p53[3]);
      if(but3p53[1] == 0){  // Button is pressed
        but3p53[2] = 1;     // On indication
      }else{
        if(but3p53[3] == 1)   // Button was released
          but3p53[2] = 0;     // Off indication
      }   
    }
    // LabView mode
    //v1.34 JCP 12/3/18  Use cCHon and cCHof
    if (Cmode == "mLV"){
      if(SerStr == "cCHon"){
        but3p53[2] = 1; // Toggle On
      }
      if(SerStr == "cCHof"){
        but3p53[3] = 1; // Toggle Off
        //v1.35 JCP 12/3/18  Add this flag
        but3p53[2] = 0; // Toggle off
      }
   }

    // Button is pressed, or CHOn execute charge
    if (but3p53[2] == 1) { 
      chargeDisplay();
      digitalWrite(vC, HIGH); //set He charge status light
      //v1.18 JCP 11/15/18 Change Charge function: Open V2, V7 --> Rr On, Open V7, V3, V4
      digitalWrite(Rr, HIGH); // Recirculator
      digitalWrite(v7, HIGH); // Valve 7
      digitalWrite(v3, HIGH); // Valve 3
      digitalWrite(v4, HIGH); // Valve 4
    }
    // Button is released, or CHOf discontinue charge
    if (but3p53[3] == 1) {
      digitalWrite(vC, LOW); //set He charge status light off
      //v1.18 JCP 11/15/18 Change Charge function: Open V2, V7 --> Rr On, Open V7, V3, V4
      digitalWrite(Rr, LOW); // Recirculator
      digitalWrite(v7, LOW); // Valve 7
      digitalWrite(v3, LOW); // Valve 3
      digitalWrite(v4, LOW); // Valve 4
      but3p53[3] = 0;
      valveDisplay(SystemStatus);
      IIcharge = 0;
      pressureDisplay();
    }
  }

  //
  //evacuate system
  //
  Bpsh = butDebounce(but1p49[0], but1p49[1], but1p49[2], but1p49[3]);
  if ((Bpsh == true && Cmode == "mAR") || SerStr == "LAcEV") {
    if (SerStr == "LAcEV") {
      but1p49[2] = !but1p49[2];
    }
    if (but1p49[2] == true) {
      m1evacMan();
      but1p49[2] = 0; //reset button
      but1p50[2] = 0; //reset cancel
    }
  }

  //
  // toggle recirculator
  // v1.27 JCP 11/25/18 Modify Toggle recirculate button operation
  //
  // Only if Charge mode is manual
  // v1.34 12/3/18 JCP Manual or Automatic
  Bpsh = butDebounce(butSp23[0], butSp23[1], butSp23[2], butSp23[3]);
  if ((Bpsh == true && Cmode == "mAR") || SerStr == "LAcTR") {
    if (SerStr == "LAcTR") {
      butSp23[2] = !butSp23[2]; // Toggle state
      butSp23[3] = 1;           // Button was pushed
    }
    toggleRecirculator();
  }


  //purge system
  Bpsh = butDebounce(but2p48[0], but2p48[1], but2p48[2], but2p48[3]);
  if ((Bpsh == true && Cmode == "mAR") || SerStr == "LAcPG") {
    if (SerStr == "LAcPG") {
      but2p48[2] = !but2p48[2];
    }
    if (but2p48[2] == true) {
      m2purgeMan();
      but2p48[2] = 0; //reset button
      but1p50[2] = 0; //reset cancel
    }
  }


  //start cooling
  Bpsh = butDebounce(but3p47[0], but3p47[1], but3p47[2], but3p47[3]);
  if ((Bpsh == true && Cmode == "mAR") || SerStr == "LAcCO") {
    if (SerStr == "LAcCO") {
      but3p47[2] = !but3p47[2];
    }
    if (but3p47[2] == true) {
      m3startCool();
      but3p47[2] = 0; //reset button
      but1p50[2] = 0; //reset cancel
    }
  }


  //timing adjust
  Bpsh = butDebounce(butEp51[0], butEp51[1], butEp51[2], butEp51[3]);
   if ((Bpsh == true && Cmode == "mAR") || SerStr == "LAdTI") {
    SerStrCheck = "";
    delay(100);
    String SerStrHold = "";

    //for(int kk=1;kk<=20;kk++){
    while (Serial1.available() && inChar != '*') {      //reads commands from Serial (LabVIEW)
      char inChar = (char)Serial1.read();  //reads current character at serial port
      SerBuf += inChar;                   //appends character to the Serial String
      if (inChar == '*') {
// v1.33 12/3/18 JCP   Remove SerStr display      
//        lcd.setCursor(0, 0); lcd.print("      ");
//        lcd.setCursor(0, 0);
        SerStr = SerBuf.substring(0, SerBuf.length() - 1);
//        lcd.print(SerStr);
        SerBuf = "";
        //lcd.print(kk);
      }
    }
    delay(50);

/* 
    lcd.setCursor(0, 0); lcd.print("               ");
    lcd.setCursor(0, 0); lcd.print("bg");
    delay(2000);
    lcd.print(SerStr);
    //delay(2000);
    lcd.print(SerStr.substring(0, 3));
    //delay(2000);
    lcd.print("dn");
*/
    if (butEp51[2] == true || SerStr.substring(0, 3) == "dTI") {
      if (SerStr.substring(0, 3) == "dTI") {    //if changed in labview
        lcd.print("inside");
        delay(1000);
        String strT1 = SerStr.substring(3, 8);
        String strT2 = SerStr.substring(9, 14);
        String strP = SerStr.substring(16, 20);
        String strW = SerStr.substring(21, 22);
        t1 = strT1.toInt();
        t2 = strT2.toInt();
        aPressure = strP.toInt();
        aWarn = strW.toInt();
      } else {
        timeDisplay();
        tAdjust();
      }
      //chargeModeToggle();
      butEp51[2] = 0; //reset button
      but1p50[2] = 0; //reset cancel
      //when finished editing go back to valve display
      coldedgeDisplay();
      valveDisplay(SystemStatus);
      delay(500);

      //save new values to EEPROM
      EEPROM.write(0, t1);
      EEPROM.write(1, t2);
      EEPROM.write(2, aPressure);
      if (ChargeMode == "Man") {
        EEPROM.write(3, 0);
      } else {
        EEPROM.write(3, 1);
      }

      EEPROM.write(4, aWarn);

      //send timing to labview
      String sndString = String();
      //sndString="dTILV"+String(t1)+"/"+String(t2); //send command dTI<t1>/<t2>
      sndString = "dTILV/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode + "/" + aWarn;
      Serial1.print(sndString);
      delay(100);
    }
  }

  //
  // Sets default values to EEPROM
  // 
  Bpsh = butDebounce(butRp44[0], butRp44[1], butRp44[2], butRp44[3]);
  if (Bpsh == true) {
    lcd.setCursor(0, 0);
    lcd.print("yes");
    EEPROM.write(0, t1Default);
    EEPROM.write(1, t2Default);
    EEPROM.write(2, aPressureDefault);
    if (ChargeModeDefault == "Man") {
      EEPROM.write(3, 0);
    } else {
      EEPROM.write(3, 1);
    }

    EEPROM.write(4, aWarnDefault);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("EEPROM RESET!");
    lcd.setCursor(0, 1);
    lcd.print("   RESET ARDUINO");
    lcd.setCursor(0, 2);
    lcd.print("      RESET LABVIEW");
    delay(-1);
  }
 
  //Autocool system

  Bpsh = butDebounce(but5p45[0], but5p45[1], but5p45[2], but5p45[3]);
  if ((Bpsh == true && Cmode == "mAR") || SerStr == "LAcAC") {
    bool cancelAC = false;  //initialize variable to test if cancelling autocool
    if (SerStr == "LAcAC") {
      but5p45[2] = !but5p45[2];
    }
    if (but5p45[2] == true) {
      m1evacAuto();
      if (but4p46[2] == 1) {
        but5p45[2] = 0; //reset button
        but4p46[2] = 0; //reset stop cool
        cancelAC = true; // set autocool to stop
        Serial1.print("acmST"); //tell labview that autocool aborted
      }
      if (but1p50[2] == 1) {
        but1p50[2] = 0; //reset cancel
        //Serial1.print("acmCA"); //tell labview that autocool cancelled
      }

      if (cancelAC == false || SerStr == "LacST") {
        m2purgeAuto();
        if (but4p46[2] == 1) {
          but5p45[2] = 0; //reset button
          but4p46[2] = 0; //reset stop cool
          but1p50[2] = 0; //reset cancel
          cancelAC = true; //set autocool to stop
          Serial1.print("acmST"); //tell labview autocool aborted
        }
        if (cancelAC == false) {
          Serial1.print("acmCO"); //tell labview that autocool is cooling
          m3startCool();
        }
      }
      but5p45[2] = 0; //reset button
      but4p46[2] = 0; //reset stop cool
    }
  }

  //if initialization is requested from labview, send back initialization parameters
  if (SerStr == "LAINI") {
    String sndString = String();
    sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode + "/" + aWarn; //send command dTI<t1>/<t2>
    Serial1.print(sndString);
  }

  //set valve 1
  if (SerStr == "LAcV1") {
    lcd.setCursor(1, 1);
    if (digitalRead(v1) == HIGH) {
      valvetoggle(v1, false);
    } else {
      valvetoggle(v1, true);
    }
  }

  //set valve 2
  if (SerStr == "LAcV2") {
    lcd.setCursor(1, 1);
    if (digitalRead(v2) == HIGH) {
      valvetoggle(v2, false);
    } else {
      valvetoggle(v2, true);
    }
  }

  //set valve 3
  if (SerStr == "LAcV3") {
    lcd.setCursor(1, 1);
    if (digitalRead(v3) == HIGH) {
      valvetoggle(v3, false);
    } else {
      valvetoggle(v3, true);
    }
  }

  //set valve 4
  if (SerStr == "LAcV4") {
    lcd.setCursor(1, 1);
    if (digitalRead(v4) == HIGH) {
      valvetoggle(v4, false);
    } else {
      valvetoggle(v4, true);
    }
  }

  //set valve 5
  if (SerStr == "LAcV5") {
    if (digitalRead(v5) == HIGH) {
      valvetoggle(v5, false);
    } else {
      valvetoggle(v5, true);
    }
  }

  //set valve 6
  if (SerStr == "LAcV6") {
    if (digitalRead(v6) == HIGH) {
      valvetoggle(v6, false);
    } else {
      valvetoggle(v6, true);
    }
  }

  //set valve 7
  if (SerStr == "LAcV7") {
    if (digitalRead(v7) == HIGH) {
      valvetoggle(v7, false);
    } else {
      valvetoggle(v7, true);
    }
  }

} //end buttonSensing routine

//
// butDebounce - Check for a button press
// Input: Pin Number - Arduino pin number or name
//        Lst  - Previous value - 0/1
//        Bst  - Toggle state
//        Bset - Button has been pushed (nor used)
//
// Output: boolean true or false, 
//  
boolean butDebounce(int pinNum, int &Lst, int &Bst , int &Bset) {

  int readPin = digitalRead(pinNum);  //variable reads current state of button

  // Button state is changed
  if(readPin != Lst){   
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
// End routine --------------------------------------------------------------------

// Start routine ------------------------------------------------------------------
void valvetoggle(int vnum, bool state) {
  digitalWrite(vnum, state);
  valveDisplay(SystemStatus);
}
// End routine --------------------------------------------------------------------


// Start routine ------------------------------------------------------------------
//
//  Toggle Recirculator
//
//  butSp23[2] is the toggle indication
//   1 -> Display water warning, turn on Recirculator, Open V4, wait 10 seconds and open V1 
//   0 -> Close V1, wait 60 sec (or until Cancel), then close V4, turn off Recirculator
//
//v1.27 JCP 11/25/18 Modify Toggle recirculate button operation
//v1.28 JCP 11/26/18 Add countdown timer display to Recirc Toggle
//
void toggleRecirculator() {
  
  bool Bpsh;
  String SerStrCancel = "";
  char inCharCancel = ' ';
  SerStrCancel.reserve(500);
  String SerBufCancel = "";
  long BeginTime, EndTime, TempLong;

  // Recirc Toggle On
  if(butSp23[2] == 1){
    //send user warning
    if(aWarn == 1) {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Please ensure that");
      lcd.setCursor(0, 1); lcd.print("cooling water is on");
      lcd.setCursor(0, 2); lcd.print("____________________");
      lcd.setCursor(0, 3); lcd.print("proceed press cancel");

      //loop to test for  dismissal of warning message
      for (int tt = 1; tt > 0; tt++) { //fast loop for sensing button press (looptime = dtc)

        //check for cancel from labview to dismiss warning
        while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
          char inCharCancel = (char)Serial1.read();  //reads current character at serial port
          SerBufCancel += inCharCancel;              //appends character to the Serial String
          if (inCharCancel == '*') {
            lcd.setCursor(14, 3); lcd.print("      ");
            lcd.setCursor(14, 3); lcd.print(SerBufCancel);
            SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
            SerBufCancel = "";
          }
        }//endwhile

        //check button for cancel to dismiss warning
        if (Cmode == "mAR") {
          Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
        }
        lcd.setCursor(0, 0); lcd.print(SerStr);
        //if dismissed break loop
        if (Bpsh == true || SerStrCancel == "LAcCA") {
          break;
        }
      }//endfor
    }//end if aWarn
  
    SerStrCancel = ""; //reset variable so that evac is not cancelled
    // Set Rr, open v4
    digitalWrite(Rr, HIGH);
    digitalWrite(v4, HIGH);
    valveDisplay(SystemStatus);
    //v1.28 JCP 11/26/18 Add countdown timer display to Recirc Toggle
    BeginTime = millis();               // Start millisecond
    TempLong = 10 * 1000;
    EndTime = BeginTime + TempLong;     // 10 sec later
    
    // Loop until EndTime, display time and send to LabView
    for (int tt = 0; tt < 25;tt++) {
      elapsedtimeDisplay(long(EndTime-BeginTime));
     
      delay(500);
      BeginTime += 500;
      if(millis() > EndTime)
        break;
    }   
    digitalWrite(v1, HIGH);
    butSp23[3] = 0;
  } // end Toggle On  

  // Recirc Toggle off
  if (butSp23[2] == 0) {
    but1p50[3] = 0;   // Clear the button pressed indication
    BeginTime = millis();               // Start millisecond
    digitalWrite(v1, LOW);    // Close V1
    valveDisplay(SystemStatus);
    // Wait 60 seconds or until Cancel
    //v1.28 JCP 11/26/18 Add countdown timer display to Recirc Toggle
    TempLong = 500;
    EndTime = 60000;  // 60 sec later
    // Loop until EndTime, display time and send to LabView
    //v1.28 JCP 11/26/18 Add countdown timer display to Recirc Toggle
    for (int tt = 0; tt < 120; tt++) {
      //check for cancel from labview to dismiss warning
      while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCancel = (char)Serial1.read();  //reads current character at serial port
        SerBufCancel += inCharCancel;              //appends character to the Serial String
        if (inCharCancel == '*') {
          lcd.setCursor(14, 3); lcd.print("      ");
          lcd.setCursor(14, 3); lcd.print(SerBufCancel);
          SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
          SerBufCancel = "";
        }
      }//endwhile

      // Check for Cancel to dismiss warning
      if (Cmode == "mAR") {
        Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      }
// v1.34 12/3/18 JCP      
//      lcd.setCursor(0, 0); lcd.print(SerStr);
      //if dismissed break loop
      if (but1p50[3] == true || SerStrCancel == "LAcCA") {
        break;
      }
      if(EndTime < 0)
        break;
      elapsedtimeDisplay(EndTime);
      delay(500);
      EndTime = EndTime - TempLong;
    }//endfor  
    
    digitalWrite(Rr, LOW);    // Turn off Recirculator
    digitalWrite(v4, LOW);    //Close V4
  }  // end Toggle Off
  valveDisplay(SystemStatus);

}  // end toggleRecirculator
// End routine --------------------------------------------------------------------

// Close all valves
void closeAllValves() { 
  digitalWrite(v1, LOW);
  digitalWrite(v2, LOW);
  digitalWrite(v3, LOW);
  digitalWrite(v4, LOW);
  digitalWrite(v5, LOW);
//digitalWrite(v6, LOW);
  //v1.26 JCP 11/25/18 Close valve 7
  digitalWrite(v7, LOW);
}

// Evacuate the control panel and cryostat
void m1evacMan() {
  bool Bpsh = false;
  bool Bpsh2 = false;
  String SerStrCancel = "";
  char inCharCancel = ' ';
  SerStrCancel.reserve(500);
  String SerBufCancel = "";
  SystemStatus = "Evacuating manifold";

  //send user warning
  if (aWarn == 1) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Please ensure that");
    lcd.setCursor(0, 1); lcd.print("manifold pump is on");
    lcd.setCursor(0, 2); lcd.print("____________________");
    lcd.setCursor(0, 3); lcd.print("proceed press cancel");

    //loop to test for  dismissal of warning message
    for (int tt = 1; tt > 0; tt++) { //fast loop for sensing button press (looptime = dtc)

      //check for cancel from labview to dismiss warning
      while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCancel = (char)Serial1.read();  //reads current character at serial port
        SerBufCancel += inCharCancel;              //appends character to the Serial String
        if (inCharCancel == '*') {
          lcd.setCursor(14, 3); lcd.print("      ");
          lcd.setCursor(14, 3); lcd.print(SerBufCancel);
          SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
          SerBufCancel = "";
        }
      }//endwhile

      //check button for cancel to dismiss warning
      if (Cmode == "mAR") {
        Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      }
// v1.31 12/1/18 JCP
//      lcd.setCursor(0, 0);
//      lcd.print(SerStr);

      //if dismissed break loop
      if (Bpsh == true || SerStrCancel == "LAcCA") {
        break;
      }
    }//endfor

  }//end if aWarn
  SerStrCancel = ""; //reset variable so that evac is not cancelled

  //start loop do stuff
  closeAllValves();
  digitalWrite(v5, HIGH);
  digitalWrite(v2, HIGH);
  digitalWrite(v3, HIGH);

  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1); lcd.print("Evacuating");
  lcd.setCursor(10, 2); lcd.print("Manifold");

  long tot_time = 0;

  for (int t = 1; t <= 2; t) { //slow loop for determining when to print time to LCD
    for (int tt = 1; tt <= dtc / dt; tt++) { //fast loop for sensing button press (looptime = dtc)

      while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCancel = (char)Serial1.read();  //reads current character at serial port
        SerBufCancel += inCharCancel;                   //appends character to the Serial String
        if (inCharCancel == '*') {
          lcd.setCursor(14, 3); lcd.print("      ");
          lcd.setCursor(14, 3); lcd.print(SerBufCancel);
          SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
          SerBufCancel = "";
        }
      }

      Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      delay(dt);

      if (Bpsh == true || SerStrCancel == "LAcCA") { //break tt for loop if cancel button press
        break;
      }//endif

      //check to see if AR/LV mode is changed while in evac mode
      Bpsh2 = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
      if (Bpsh2 == true || SerStrCancel == "LAmTO") { //break tt for loop if cancel button press
        if (SerStrCancel == "LAmTO") {
          but2p52[2] = !but2p52[2]; // Toggle
        }
        CheckLVtoggle("RUNev");
        //re-display the mode on display.
        lcd.setCursor(10, 1); lcd.print("Evacuating");
        lcd.setCursor(10, 2); lcd.print("Manifold");
      }//endif

      if (SerStrCancel == "LAINI") {
        String sndString = String();
        sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode; //send command dTI<t1>/<t2>
        Serial1.print(sndString);
      }

      SerStrCancel = " "; //critical to reset the serial string read. otherwise when mode change, will cause continuous toggle.
    }//endfor tt

    if (Bpsh == true || SerStrCancel == "LAcCA") { //break t for loop if cancel button press
      brk = true;
      brk = false;
      break;
    }//endif

    tot_time += dtc; //add iteration to elapsed time
    elapsedtimeDisplay(tot_time);

  }//endfor

  //end loop do stuff
  digitalWrite(v5, LOW);
  digitalWrite(v2, LOW);
  digitalWrite(v3, LOW);
  //but1p49[2]=0;
  SystemStatus = "Idle";
  valveDisplay(SystemStatus);
} //end m1evacMan()

//
// Evacuate the control panel and cryostat but with timing
//
void m1evacAuto() {       
  bool Bpsh = false;
  bool Bpsh2 = false;
  SystemStatus = "Auto cool: evacuating manifold";

  String SerStrCancel = "";
  char inCharCancel = ' ';
  SerStrCancel.reserve(500);
  String SerBufCancel = "";

  //send user warning
  if (aWarn == 1) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Check helium charged");
    lcd.setCursor(0, 1); lcd.print("Check H2O/Recirc. on");
    lcd.setCursor(0, 2); lcd.print("____________________");
    lcd.setCursor(0, 3); lcd.print("proceed press cancel");

    //loop to test for  dismissal of warning message
    for (int tt = 1; tt > 0; tt++) { //fast loop for sensing button press (looptime = dtc)

      //check for cancel from labview to dismiss warning
      while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCancel = (char)Serial1.read();  //reads current character at serial port
        SerBufCancel += inCharCancel;                   //appends character to the Serial String
        if (inCharCancel == '*') {
          lcd.setCursor(14, 3); lcd.print("      ");
          lcd.setCursor(14, 3); lcd.print(SerBufCancel);
          SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
          SerBufCancel = "";
        }
      }//endwhile

      //check button for cancel to dismiss warning
      if (Cmode == "mAR") {
        Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      }
// v1.33 12/3/18 JCP   Remove SerStr display      
//      lcd.setCursor(0, 0);
//      lcd.print(SerStr);

      //if dismissed break loop
      if (Bpsh == true || SerStrCancel == "LAcCA") {
        break;
      }
    }//endfor

  }//end if aWarn
  SerStrCancel = ""; //reset variable so that evac is not cancelled

  //start loop do stuff
  closeAllValves();
  digitalWrite(v5, HIGH);
  digitalWrite(v2, HIGH);
  digitalWrite(v3, HIGH);

  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1); lcd.print("Evacuating");
  lcd.setCursor(10, 2); lcd.print("Manifold");

  long tot_time = t1 * 60.0 * 1000;

  for (int t = 1; t <= t1 * 60 * 1000 / dtc; t++) { //slow loop for determining when to print time to LCD
    for (int tt = 1; tt <= dtc / dt; tt++) { //fast loop for sensing button press (looptime = dtc
      Bpsh = butDebounce(but4p46[0], but4p46[1], but4p46[2], but4p46[3]); //check stop cool button hit
      if (Bpsh == 0) {
        Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]); //check cancel/return button hit
      }
      delay(dt);

      if (Bpsh == true || SerStr == "LAcCA" || SerStr == "LAcST") { //break tt for loop if cancel button press
        if (SerStr == "LAcCA") {
          but1p50[2] = 1;
        }
        if (SerStr == "LAcST") {
          but4p46[2] = 1;
        }
        break;
      }//endif
      SerStr = ""; //critical to reset the serial string read. otherwise when mode change, will cause continuous toggle.

      while (Serial1.available() && inChar != '*') {      //reads commands from Serial (LabVIEW)
        char inChar = (char)Serial1.read();  //reads current character at serial port
        SerBuf += inChar;                   //appends character to the Serial String
        if (inChar == '*') {
// v1.33 12/3/18 JCP   Remove SerStr display      
//          lcd.setCursor(0, 0); lcd.print("      ");
//          lcd.setCursor(0, 0);
          SerStr = SerBuf.substring(SerBuf.length() - 6, SerBuf.length() - 1);
//          lcd.print(SerStr);
          SerBuf = "";
        }
      }

      //check to see if AR/LV mode is changed while in evac mode
      Bpsh2 = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
      if (Bpsh2 == true || SerStr == "LAmTO") { //break tt for loop if cancel button press
        if (SerStr == "LAmTO") {
          but2p52[2] = !but2p52[2]; // Toggle
        }
        CheckLVtoggle("RUNev");
        lcd.setCursor(10, 1); lcd.print("Evacuating");
        lcd.setCursor(10, 2); lcd.print("Manifold");
      }//endif

      if (SerStr == "LAINI") {
        String sndString = String();
        sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode; //send command dTI<t1>/<t2>
        Serial1.print(sndString);
      }
    }//endfor tt


    if (Bpsh == true || SerStr == "LAcCA" || SerStr == "LAcST") { //break t for loop if cancel button press
      brk = true;
      brk = false;
      break;
    }//endif

    tot_time -= dtc; //add iteration to elapsed time
    if ((tot_time % dtc) == 0) {
      elapsedtimeDisplay(tot_time);
    }
  }//endfor t

  //end loop do stuff
  digitalWrite(v5, LOW);
  digitalWrite(v2, LOW);
  digitalWrite(v3, LOW);
  //but1p49[2]=0;
  SystemStatus = "Idle";
  valveDisplay(SystemStatus);
  delay(500);
} //end m1evacMan()


//
// Purge cycle
//
void m2purgeMan() {      
  bool Bpsh = false;
  bool Bpsh2 = false;

  String SerStrCancel = "";
  char inCharCancel = ' ';
  SerStrCancel.reserve(500);
  String SerBufCancel = "";
  SystemStatus = "Purging manifold";

  //send user warning
  if (aWarn == 1) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Please ensure that");
    lcd.setCursor(0, 1); lcd.print("gas cylinder is open");
    lcd.setCursor(0, 2); lcd.print("____________________");
    lcd.setCursor(0, 3); lcd.print("proceed press cancel");

    //loop to test for  dismissal of warning message
    for (int tt = 1; tt > 0; tt++) { //fast loop for sensing button press (looptime = dtc)

      //check for cancel from labview to dismiss warning
      while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCancel = (char)Serial1.read();  //reads current character at serial port
        SerBufCancel += inCharCancel;                   //appends character to the Serial String
        if (inCharCancel == '*') {
          lcd.setCursor(14, 3);
          lcd.print("      ");
          lcd.setCursor(14, 3);
          lcd.print(SerBufCancel);
          SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
          SerBufCancel = "";
        }
      }//endwhile

      //check button for cancel to dismiss warning
      if (Cmode == "mAR") {
        Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      }
// v1.31 12/1/18 JCP
//      lcd.setCursor(0, 0);
//      lcd.print(SerStr);

      //if dismissed break loop
      if (Bpsh == true || SerStrCancel == "LAcCA") {
        break;
      }
    }//endfor

  }//end if aWarn
  SerStrCancel = ""; //reset variable so that evac is not cancelled

  //start loop do stuff
  closeAllValves();
  digitalWrite(v7, HIGH);  //open charge solenoid
  digitalWrite(v5, HIGH);  //open vacuum solenoid
  digitalWrite(v2, HIGH);  //open charge supply solenoid

  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1); lcd.print("Purge");
  lcd.setCursor(10, 2); lcd.print("Manifold");

  long tot_time = 0;
  for (int t = 1; t <= t2 * 60 * 1000 / dtc; t) { //slow loop for determining when to print time to LCD
    for (int tt = 1; tt <= dtc / dt; tt++) { //fast loop for sensing button press (looptime = dtc)

      while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCancel = (char)Serial1.read();  //reads current character at serial port
        SerBufCancel += inCharCancel;                   //appends character to the Serial String
        if (inCharCancel == '*') {
          lcd.setCursor(14, 3);
          lcd.print("      ");
          lcd.setCursor(14, 3);
          lcd.print(SerBufCancel);
          SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
          SerBufCancel = "";
        }
      }

      Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      delay(dt);

      if (Bpsh == true || SerStrCancel == "LAcCA") { //break tt for loop if cancel button press
        //brk=true;
        //brk=false;
        break;
      }//endif

      //check to see if AR/LV mode is changed while in evac mode
      Bpsh2 = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
      if (Bpsh2 == true || SerStrCancel == "LAmTO") { //break tt for loop if cancel button press
        if (SerStrCancel == "LAmTO") {
          but2p52[2] = !but2p52[2];   // Toggle 
        }
        CheckLVtoggle("RUNpu");
        lcd.setCursor(10, 1); lcd.print("Purge");
        lcd.setCursor(10, 2); lcd.print("Manifold");
      }//endif

      if (SerStrCancel == "LAINI") {
        String sndString = String();
        sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode; //send command dTI<t1>/<t2>
        Serial1.print(sndString);
      }

      SerStrCancel = " "; //critical to reset the serial string read. otherwise when mode change, will cause continuous toggle.

    }//endfor tt

    if (Bpsh == true || SerStrCancel == "LAcCA") { //break t for loop if cancel button press
      brk = true;
      brk = false;
      but2p48[2] = 0; //reset button
      break;
    }//endif

    tot_time += dtc; //add iteration to elapsed time
    elapsedtimeDisplay(tot_time);

  }//endfor

  //end loop do stuff
  digitalWrite(v7, LOW);
  digitalWrite(v5, LOW);
  digitalWrite(v2, LOW);
  //but1p49[2]=0;
  SystemStatus = "Idle";
  valveDisplay(SystemStatus);
} //end m2purgeMan()

//
// Purge cycle
//
void m2purgeAuto() {  
  bool Bpsh = false;
  bool Bpsh2 = false;
  SystemStatus = "Auto cool: purging manifold";
  //start loop do stuff
  closeAllValves();
  digitalWrite(v7, HIGH);  //open charge solenoid
  digitalWrite(v5, HIGH);  //open vacuum solenoid
  digitalWrite(v2, HIGH);  //open charge supply solenoid
  //digitalWrite(Rr,HIGH);   //start warming the recirculator

  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1); lcd.print("Purge");
  lcd.setCursor(10, 2); lcd.print("Manifold");

  long tot_time = t2 * 60 * 1000;
  SerStr = ""; //clear serial string, so doesn't accidently break if evac cancelled
  for (int t = 1; t <= t2 * 60 * 1000 / dtc; t++) { //slow loop for determining when to print time to LCD
    for (int tt = 1; tt <= dtc / dt; tt++) { //fast loop for sensing button press (looptime = dtc)
      Bpsh = butDebounce(but4p46[0], but4p46[1], but4p46[2], but4p46[3]);
      if (Bpsh == 0) {
        Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      }
      delay(dt);

      if (Bpsh == true || SerStr == "LAcCA" || SerStr == "LAcST") { //break tt for loop if cancel button press
        if (SerStr == "LAcST") { //if the LV stop button pushed, set arduino stop button status
          but4p46[2] = 1;
        }
        break;
      }//endif

      SerStr = ""; //critical to reset the serial string read. otherwise when mode change, will cause continuous toggle.

      while (Serial1.available() && inChar != '*') {      //reads commands from Serial (LabVIEW)
        char inChar = (char)Serial1.read();  //reads current character at serial port
        SerBuf += inChar;                   //appends character to the Serial String
        if (inChar == '*') {
// v1.33 12/3/18 JCP   Remove SerStr display      
//          lcd.setCursor(0, 0); lcd.print("      ");
//          lcd.setCursor(0, 0);
          SerStr = SerBuf.substring(SerBuf.length() - 6, SerBuf.length() - 1);
//          lcd.print(SerStr);
          SerBuf = "";
        }
      }

      //check to see if AR/LV mode is changed while in evac mode
      Bpsh2 = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
      if (Bpsh2 == true || SerStr == "LAmTO") { //break tt for loop if cancel button press
        if (SerStr == "LAmTO") {
          but2p52[2] = !but2p52[2];
        }
        CheckLVtoggle("RUNpu");
        lcd.setCursor(10, 1); lcd.print("Purge");
        lcd.setCursor(10, 2); lcd.print("Manifold");
      }//endif

      if (SerStr == "LAINI") {
        String sndString = String();
        sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode; //send command dTI<t1>/<t2>
        Serial1.print(sndString);
      }
    }//endfor tt

    if (Bpsh == true || SerStr == "LAcCA" || SerStr == "LAcST") { //break t for loop if cancel button press
      brk = true;
      brk = false;
      but2p48[2] = 0; //reset button
      break;
    }//endif

    tot_time -= dtc; //add iteration to elapsed time
    if ((tot_time % dtc) == 0) {
      elapsedtimeDisplay(tot_time);
    }

  }//endfor

  //end loop do stuff
  digitalWrite(v7, LOW);
  digitalWrite(v5, LOW);
  digitalWrite(v2, LOW);
  //but1p49[2]=0;
  SystemStatus = "Idle";
  valveDisplay(SystemStatus);
} //end m2purgeAuto()

//
// Start cooling
//
void m3startCool() {  
  bool Bpsh = false;
  bool Bpsh2 = false;
  String SerStrCancel = "";
  char inCharCancel = ' ';
  SerStrCancel.reserve(500);
  String SerBufCancel = "";

  //send user warning
  if (aWarn == 1) {
    lcd.clear();
    lcd.setCursor(0, 0); //lcd.print("Check helium charged");
    lcd.setCursor(0, 1); lcd.print("Check H2O/Recirc. on");
    lcd.setCursor(0, 2); lcd.print("____________________");
    lcd.setCursor(0, 3); lcd.print("proceed press cancel");

    //loop to test for  dismissal of warning message
    for (int tt = 1; tt > 0; tt++) { //fast loop for sensing button press (looptime = dtc)

      //check for cancel from labview to dismiss warning
      while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCancel = (char)Serial1.read();  //reads current character at serial port
        SerBufCancel += inCharCancel;                   //appends character to the Serial String
        if (inCharCancel == '*') {
          lcd.setCursor(14, 3); lcd.print("      ");
          lcd.setCursor(14, 3); lcd.print(SerBufCancel);
          SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
          SerBufCancel = "";
        }
      }//endwhile

      //check button for cancel to dismiss warning
      if (Cmode == "mAR") {
        Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      }
// v1.34 12/3/18 JCP      
//      lcd.setCursor(0, 0);
//      lcd.print(SerStr);

      //if dismissed break loop
      if (Bpsh == true || SerStrCancel == "LAcCA") {
        break;
      }
    }//endfor

  }//end if aWarn
  SerStrCancel = ""; //reset variable so that evac is not cancelled
 
  SystemStatus = "Cooling: return open";

  //start loop do stuff
  closeAllValves();
  digitalWrite(Rr, HIGH);
  digitalWrite(v4, HIGH);
  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1);
  lcd.print("Cooling");
  lcd.setCursor(10, 2);
  lcd.print("System");

  long tot_time = 10000;
  for (int t = 1; t <= 10000 / dtc; t++) { //slow loop for determining when to print time to LCD
    for (int tt = 1; tt <= dtc / dt; tt++) { //fast loop for sensing button press (looptime = dtc)
      Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      delay(dt);

      if (Bpsh == true) { //break tt for loop if cancel button press
        //brk=true;
        //brk=false;
        break;
      }//endif

    }//endfor tt

    if (Bpsh == true) { //break t for loop if cancel button press
      brk = true;
      brk = false;
      break;
    }//endif

    tot_time -= dtc; //add iteration to elapsed time
    if ((tot_time % dtc) == 0) {
      elapsedtimeDisplay(tot_time);
    }
  }//endfor t

  SystemStatus = "System cooling";
  //end loop do stuff
  digitalWrite(v1, HIGH);
  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1);
  lcd.print("Cooling");
  lcd.setCursor(10, 2);
  lcd.print("System");
  delay(5000);

  if (ChargeMode == "Auto") {
    AutoCharge();
  }

  digitalWrite(Rc, HIGH); //start compressor
  delay(100);
  valveDisplay(SystemStatus);
  delay(100);
  IIcharge = 0;
  pressureDisplay();

  //loop continuously in cooling mode until user hits stop cooling button
  Bpsh = butDebounce(but4p46[0], but4p46[1], but4p46[2], but4p46[3]);
  while (Bpsh == false && SerStr != "LAcST") {
    SerStr = " "; //critical to reset the serial string read. otherwise when mode change, will cause continuous toggle.
    while (Serial1.available() && inChar != '*') {      //reads commands from Serial (LabVIEW)
      char inChar = (char)Serial1.read();  //reads current character at serial port
      SerBuf += inChar;                   //appends character to the Serial String
      if (inChar == '*') {
        lcd.setCursor(14, 3); lcd.print("      ");
        lcd.setCursor(14, 3);
        SerStr = SerBuf.substring(SerBuf.length() - 6, SerBuf.length() - 1);
        lcd.print(SerStr);
        SerBuf = "";
      }
    }
    Bpsh = butDebounce(but4p46[0], but4p46[1], but4p46[2], but4p46[3]);

    //check to see if AR/LV mode is changed while in evac mode
    Bpsh2 = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
    if (Bpsh2 == true || SerStr == "LAmTO") { //break tt for loop if cancel button press
      if (SerStr == "LAmTO") {
        but2p52[2] = !but2p52[2];
      }
      CheckLVtoggle("RUNco");
      lcd.setCursor(10, 1);
      lcd.print("Cooling");
      lcd.setCursor(10, 2);
      lcd.print("System");
    }//endif

    if (SerStr == "LAINI") {
      String sndString = String();
      sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode; //send command dTI<t1>/<t2>
      Serial1.print(sndString);
    }
  }//endwhile Bpsh
  
  SerStr = ""; //reset SerStr
  valveDisplay(SystemStatus);
  m3stopCool();

} //end m3startCool()


// Stop cooling
void m3stopCool() {
  //start loop do stuff
  bool Bpsh = false;
  bool Bpsh2 = false;
  SystemStatus = "Warming: closed supply";
  digitalWrite(Rc, LOW);  //turn off the compressor
  digitalWrite(v1, LOW);  //close the He supply valve
  digitalWrite(v2, HIGH);
  digitalWrite(v3, HIGH);
  delay(1000);
  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1); lcd.print("Warming");
  lcd.setCursor(10, 2); lcd.print("System");

  long tot_time = valvedelay;
  for (int t = 1; t <= valvedelay / dtc; t++) { //slow loop for determining when to print time to LCD
    for (int tt = 1; tt <= dtc / dt; tt++) { //fast loop for sensing button press (looptime = dtc)
      Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      delay(dt);

      if (Bpsh == true) { //break tt for loop if cancel button press
        break;
      }//endif

    }//endfor tt

    if (Bpsh == true) { //break t for loop if cancel button press
      brk = true;
      brk = false;
      break;
    }//endif

    tot_time -= dtc; //add iteration to elapsed time
    if ((tot_time % dtc) == 0) {
      elapsedtimeDisplay(tot_time);
    }
  }//endfor t

  SystemStatus = "Warming, START EVACUATION";
  //end loop do stuff
  digitalWrite(v2, LOW);
  digitalWrite(v3, LOW);
  digitalWrite(v4, LOW);
  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1); lcd.print("Warming");
  lcd.setCursor(10, 2); lcd.print("System");
  delay(5000);
  digitalWrite(Rr, LOW);

  Bpsh = butDebounce(but1p49[0], but1p49[1], but1p49[2], but1p49[3]);
  while (Bpsh == false && SerStr != "LAcCA") {
    SerStr = " "; //critical to reset the serial string read. otherwise when mode change, will cause continuous toggle.

    //run routine waiting for either evac button push or LAcEV command from LV, while YES displaying "start vac"
    Bpsh = butDebounce(but1p49[0], but1p49[1], but1p49[2], but1p49[3]);
    lcd.setCursor(10, 3);
    lcd.print("START VAC!");

    for (int tt = 1; tt <= 500 / dt; tt++) { //fast loop for sensing button press (looptime = dtc)
      while (Serial1.available() && inChar != '*') {      //reads commands from Serial (LabVIEW)
        char inChar = (char)Serial1.read();  //reads current character at serial port
        SerBuf += inChar;                   //appends character to the Serial String
        if (inChar == '*') {
          lcd.setCursor(14, 3); lcd.print("      ");
          lcd.setCursor(14, 3);
          SerStr = SerBuf.substring(SerBuf.length() - 6, SerBuf.length() - 1);
          lcd.print(SerBuf);
          SerBuf = "";
        }
      }

      if (SerStr == "LAcEV") { //if command evac command sent from LV send then set button
        but1p49[2] = !but1p49[2];
      }
      Bpsh = butDebounce(but1p49[0], but1p49[1], but1p49[2], but1p49[3]);
      delay(dt);

      if (Bpsh == true || SerStr == "LAcEV") { //break tt for loop if Evac button press
        break;
      }//endif

      //check to see if AR/LV mode is changed while in evac mode
      Bpsh2 = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
      if (Bpsh2 == true || SerStr == "LAmTO") { //break tt for loop if cancel button press
        if (SerStr == "LAmTO") {
          but2p52[2] = !but2p52[2];
        }
        CheckLVtoggle("RUNwa");
        lcd.setCursor(10, 1); lcd.print("Warming");
        lcd.setCursor(10, 2); lcd.print("System");
      }//endif

      if (SerStr == "LAINI") {
        String sndString = String();
        sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode; //send command dTI<t1>/<t2>
        Serial1.print(sndString);
      }
    }//endfor tt

    if (Bpsh == true || SerStr == "LAcEV") { //break tt for loop if Evac button press
      brk = true;
      brk = false;
      break;
    }//endif

    SerStr = ""; //clear SerStr

    //run same routine as previous, but while NOT showing "start vac"
    lcd.setCursor(10, 3);
    lcd.print("          ");
    for (int tt = 1; tt <= 500 / dt; tt++) { //fast loop for sensing button press (looptime = dtc)
      while (Serial1.available() && inChar != '*') {      //reads commands from Serial (LabVIEW)
        char inChar = (char)Serial1.read();  //reads current character at serial port
        SerBuf += inChar;                   //appends character to the Serial String
        if (inChar == '*') {
          lcd.setCursor(14, 3); lcd.print("      ");
          lcd.setCursor(14, 3); lcd.print(SerBuf);
          SerStr = SerBuf.substring(0, SerBuf.length() - 1);
          SerBuf = "";
        }
      }

      if (SerStr == "LAcEV") { //if command evac command sent from LV send then set button
        but1p49[2] = !but1p49[2];
      }
      Bpsh = butDebounce(but1p49[0], but1p49[1], but1p49[2], but1p49[3]);
      delay(dt);

      if (Bpsh == true || SerStr == "LAcEV") { //break tt for loop if cancel button press
        break;
      }//endif

      //check to see if AR/LV mode is changed while in evac mode
      Bpsh2 = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
      if (Bpsh2 == true || SerStr == "LAmTO") { //break tt for loop if cancel button press
        if (SerStr == "LAmTO") {
          but2p52[2] = !but2p52[2];
        }
        CheckLVtoggle("RUNwa");
        lcd.setCursor(10, 1); lcd.print("Warming");
        lcd.setCursor(10, 2); lcd.print("System");
      }//endif

      if (SerStr == "LAINI") {
        String sndString = String();
        sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode; //send command dTI<t1>/<t2>
        Serial1.print(sndString);
      }

      SerStr = " "; //critical to reset the serial string read. otherwise when mode change, will cause continuous toggle.

    }//endfor tt

    if (Bpsh == true || SerStr == "LAcEV") { //break tt for loop if cancel button press
      brk = true;
      brk = false;
      break;
    }//endif
  }

  SerStr = ""; //Clear SerStr

  m1evacManIndefinite();

} //end m3stopCool()


// Evacuate indefinitely
void m1evacManIndefinite() {
  bool Bpsh = false;
  bool Bpsh2 = false;
  SystemStatus = "Warming & evacuating manifold";

  String SerStrCancel = "";
  char inCharCancel = ' ';
  SerStrCancel.reserve(500);
  String SerBufCancel = "";

  //send user warning
  if (aWarn == 1) {
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Please ensure that");
    lcd.setCursor(0, 1); lcd.print("manifold pump is on");
    lcd.setCursor(0, 2); lcd.print("____________________");
    lcd.setCursor(0, 3); lcd.print("proceed press cancel");

    //loop to test for  dismissal of warning message
    for (int tt = 1; tt > 0; tt++) { //fast loop for sensing button press (looptime = dtc)

      //check for cancel from labview to dismiss warning
      while (Serial1.available() && inCharCancel != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCancel = (char)Serial1.read();  //reads current character at serial port
        SerBufCancel += inCharCancel;                   //appends character to the Serial String
        if (inCharCancel == '*') {
          lcd.setCursor(14, 3);
          lcd.print("      ");
          lcd.setCursor(14, 3);
          lcd.print(SerBufCancel);
          SerStrCancel = SerBufCancel.substring(0, SerBufCancel.length() - 1);
          SerBufCancel = "";
        }
      }//endwhile

      //check button for cancel to dismiss warning
      if (Cmode == "mAR") {
        Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
      }
// v1.31 12/1/18 JCP
//      lcd.setCursor(0, 0);
//      lcd.print(SerStr);

      //if dismissed break loop
      if (Bpsh == true || SerStrCancel == "LAcCA") {
        break;
      }

    }//endfor
  }//end if aWarn

  SerStrCancel = ""; //reset variable so that evac is not cancelled

  //start loop do stuff
  closeAllValves();
  digitalWrite(v5, HIGH);
  digitalWrite(v2, HIGH);
  digitalWrite(v3, HIGH);

  valveDisplay(SystemStatus);
  lcd.setCursor(10, 1); lcd.print("Warm/purge");
  lcd.setCursor(10, 2); lcd.print("Manifold");

  //loop continuously in cooling mode until user cancels
  Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);
  while (Bpsh == false && SerStr != "LAcCA") {
    SerStr = " "; //critical to reset the serial string read. otherwise when mode change, will cause continuous toggle.
    while (Serial1.available() && inChar != '*') {      //reads commands from Serial (LabVIEW)
      char inChar = (char)Serial1.read();  //reads current character at serial port
      SerBuf += inChar;                   //appends character to the Serial String
      if (inChar == '*') {
        lcd.setCursor(14, 3);
        lcd.print("      ");
        lcd.setCursor(14, 3);
        SerStr = SerBuf.substring(SerBuf.length() - 6, SerBuf.length() - 1);
        lcd.print(SerStr);
        SerBuf = "";
      }
    }
    if (SerStr == "LAcCA") {
      but1p50[2] = !but1p50[2];
    }
    Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]);

    //check to see if AR/LV mode is changed while in evac mode
    Bpsh2 = butDebounce(but2p52[0], but2p52[1], but2p52[2], but2p52[3]);
    if (Bpsh2 == true || SerStr == "LAmTO") { //break tt for loop if cancel button press
      if (SerStr == "LAmTO") {
        but2p52[2] = !but2p52[2];
      }
      CheckLVtoggle("RUNev");
      lcd.setCursor(10, 1); lcd.print("Warm/purge");
      lcd.setCursor(10, 2); lcd.print("Manifold");
    }//endif

    if (SerStr == "LAINI") {
      String sndString = String();
      sndString = "ARINI/" + String(t1) + "/" + String(t2) + "/" + String(aPressure) + "/" + ChargeMode + "/" + Cmode; //send command dTI<t1>/<t2>
      Serial1.print(sndString);
    }
  }//endwhile

  SerStr = ""; //Clear SerStr

  //end loop do stuff
  digitalWrite(v5, LOW);
  digitalWrite(v2, LOW);
  digitalWrite(v3, LOW);
  //but1p49[2]=0;
  SystemStatus = "Idle";
  valveDisplay(SystemStatus);
} //end m1evacManIndefinite()

//
// elapsedtimeDisplay(long)
// Display the time on the LCD
//
// Input:  32 bit time value in milliseconds
// Output: Display is updated as hr:min:sec
//
// if only seconds: 0:xx
// if hour is > 100, only Hr:min
//
// v1.26 JCP 11/25/18 Fix time display conversion to negative number
// v1.29 JCP 11/30/18 Convert to hr:min:sec format
//

//
void elapsedtimeDisplay(long etime) { 

  if(etime < 0){
    lcd.setCursor(10, 3);
    lcd.print("Time Error");    //clear display, just in case string length changes
    return;   
  }

  long tdiv = 1000;
  long tseconds = etime / tdiv;         //calculate total # of seconds
  tdiv = 3600;
  long thours = tseconds / tdiv;        //calculate total # of hours
  tdiv = 60;
  long tminutes = (tseconds / tdiv) % tdiv;      //calculate total # of minutes
  tseconds = tseconds % tdiv;           //find remaining seconds

  //clear display, just in case string length changes
  lcd.setCursor(10, 3);
  lcd.print("          ");              

  // If more than 10 hours, just display hours and minutes and exit
  if(thours > 10.0){
    lcd.setCursor(14, 3);
    lcd.print(String(thours) + ":");
    if(tminutes < 10.0){
      lcd.print("0" + String(tminutes));
    } else {
      lcd.print(String(tminutes));
    }
    return;
  }  

  // Less than 10 hours, so display hr:min:sec as needed
  // Display Hours in 11, 12 and : in 13
  if(thours > 0){
    if(thours < 10.0){
     lcd.setCursor(12, 3);
    }else{
      lcd.setCursor(11, 3);
    }
      lcd.print(String(thours) + ":0");
  }
  
  // Display Minutes in 14, 15 and : in 16
  if (tminutes < 10.0) {
    lcd.setCursor(15, 3);
  }else{
    lcd.setCursor(14, 3);
  }
  lcd.print(String(tminutes) + ":0");
  
  // Display Seconds 17, 18
  if (tseconds < 10.0) {
    lcd.setCursor(18, 3);
  } else {
    lcd.setCursor(17, 3);
  }
  lcd.print(String(tseconds));

} // end elapsedtimeDisplay()

//
// Display the pressure on the LCD and send it to LV
//
void pressureDisplay() {
  //get pressure
  String sndString;

  if (IIcharge == 0) {  //if index is zero, display
    vlt = analogRead(Pp) * 5.0 / 1024.0; //get voltage
    PSI = vlt * 200 / 5;  //convert to psi
    lcd.setCursor(10, 0);
    lcd.print("         ");
    lcd.setCursor(10, 0);
    lcd.print(PSI);
    lcd.print(" PSI");
    IIcharge = 0;
    sndString = "ALcPR/" + String(PSI) + "*";
    Serial1.print(sndString);
  }

  IIcharge++;   //increme pressure counter

  if (IIcharge == NNcharge) { //if meet end of cycle, reset
    IIcharge = 0;
  }
} // end pressureDisplay()

//
// Display the adjustment screen
//
void timeDisplay() {
  lcd.clear();
  coldedgeDisplay();
  lcd.setCursor(0, 0); lcd.print("Adjust   "); lcd.write(4);
  lcd.setCursor(0, 1); lcd.write(7); lcd.print("te="); lcd.print(t1); lcd.print(" min");
  lcd.setCursor(0, 2); lcd.write(4); lcd.print("tp="); lcd.print(t2); lcd.print(" min");
  lcd.setCursor(0, 3); lcd.write(4); lcd.print("Cmode="); lcd.print(ChargeMode);

  lcd.setCursor(11, 1); lcd.write(4); lcd.print("P="); lcd.print(aPressure);
  lcd.setCursor(17, 1); lcd.print("PSI");

  lcd.setCursor(11, 2); lcd.write(4); lcd.print("Warn=");
  if (aWarn == 1) {
    lcd.print("y");
  } else {
    lcd.print("n");
  }

  lcd.setCursor(11, 3); lcd.write(4); lcd.print("RESET");

} // end timeDisplay()

//
// Adjust the values with the encoder
//
void tAdjust() { //send 0

  //SET t1
  int val = 0;
  String FLWta = "Continue";
  //val!=2 && val!=3
  while (true) {
    t1 += val * 1;
    if (t1 < 0) { //make sure time is never negative
      t1 = 0;
    }

    lcd.setCursor(0, 1); lcd.write(7);
    lcd.setCursor(4, 1); lcd.print("       ");
    lcd.setCursor(4, 1); lcd.print(t1); lcd.print(" min");
    //lcd.setCursor(19,3);
    //lcd.print("*");

    val = ReadEncoder();

    //lcd.setCursor(15,3);
    //lcd.print(val);
    if (val == 2) {
      FLWta = "Cancel";
      //butEp51[2]=0;
      break;
    }
    if (val == 3) {
      FLWta = "tNext";
      break;
    }
  }

  //SET t2
  val = 0; //reset val to add
  if (FLWta == "tNext") {
    FLWta = "continue";
    while (true) {
      t2 += val * 1;
      if (t2 < 0) { //make sure time is never negative
        t2 = 0;
      }
      lcd.setCursor(0, 1); lcd.write(4);
      lcd.setCursor(0, 2); lcd.write(7);
      lcd.setCursor(4, 2); lcd.print("       ");
      lcd.setCursor(4, 2); lcd.print(t2); lcd.print(" min");
      //lcd.setCursor(19,3);
      //lcd.print("*");

      val = ReadEncoder();

      if (val == 2) {
        FLWta = "Cancel";
        //butEp51[2]=0;
        break;
      }
      if (val == 3) {
        FLWta = "tNext";
        //butEp51[2]=0;
        break;
      }

    }//endwhile

  }//endif

  //SET  charge Mode
  int togglecount = 1; //keep track of even or odd for toggle between modes
  val = 0;
  String Mode1;
  String Mode2;

  if (ChargeMode == "Man") {
    Mode1 = "Man";
    Mode2 = "Auto";
  } else {
    Mode1 = "Auto";
    Mode2 = "Man";
  }

  if (FLWta == "tNext") {
    FLWta = "continue";
    while (true) {
      togglecount += val;
      lcd.setCursor(0, 2); lcd.write(4);
      lcd.setCursor(0, 3); lcd.write(7);
      lcd.setCursor(7, 3); lcd.print("    ");
      lcd.setCursor(7, 3);
      if (abs(togglecount) % 2 == 1) {
        ChargeMode = Mode1;
        lcd.print(ChargeMode);
      } else {
        ChargeMode = Mode2;
        lcd.print(ChargeMode);
      }
      val = ReadEncoder();
      if (val == 2) {
        FLWta = "Cancel";
        break;
      }
      if (val == 3) {
        FLWta = "tNext";
        break;
      }
    }//endwhile
  }//end if

  //SET Pressure
  val = 0; //reset val to add
  if (FLWta == "tNext") {
    FLWta = "continue";
    while (true) {

      aPressure += val * 1;

      if (aPressure < 0) { //make sure pressure is never negative
        aPressure = 0;
      }
      lcd.setCursor(0, 3);  lcd.write(4);
      lcd.setCursor(11, 1); lcd.write(7);
      lcd.setCursor(14, 1); lcd.print("   ");
      lcd.setCursor(14, 1); lcd.print(aPressure);
      lcd.setCursor(17, 1); lcd.print("PSI");

      val = ReadEncoder();

      if (val == 2) {
        FLWta = "Cancel";
        break;
      }
      if (val == 3) {
        FLWta = "tNext";
        break;
      }

    }//endwhile
  }//endif

  //SET Warn
  val = 0; //reset val to add
  if (FLWta == "tNext") {
    FLWta = "continue";
    while (true) {
      aWarn += val * 1;
      aWarn = aWarn % 2;

      lcd.setCursor(11, 1); lcd.write(4);
      lcd.setCursor(11, 2); lcd.write(7);
      lcd.setCursor(17, 2); lcd.print(" ");
      lcd.setCursor(17, 2);
      if (aWarn == 1) {
        lcd.print("y");
      } else {
        lcd.print("n");
      }
      //lcd.setCursor(19,3);
      //lcd.print("*");

      val = ReadEncoder();

      if (val == 2) {
        FLWta = "Cancel";
        break;
      }
      if (val == 3) {
        FLWta = "tNext";
        break;
      }

    }//endwhile
  }//endif

  //RESET TIMING
  val = 0; //reset val to add
  if (FLWta == "tNext") {
    FLWta = "continue";
    while (true) {

      if (aPressure < 0) { //make sure time is never negative
        aPressure = 0;
      }
      lcd.setCursor(11, 2); lcd.write(4);
      lcd.setCursor(11, 3); lcd.write(7);

      val = ReadEncoder();

      if (val == 2) {
        FLWta = "Cancel";
        break;
      }
      if (val == 3) {
        FLWta = "tNext";
        break;
      }

      if (val == 1) {
        t1 = t1Default;
        t2 = t2Default;
        aPressure = aPressureDefault;
        ChargeMode = ChargeModeDefault;
        timeDisplay();
        lcd.setCursor(0, 1);  lcd.write(4);
        lcd.setCursor(11, 2); lcd.write(7);
      }
    }//endwhile
  }//endif

} // end tAdjust 

//single read of encoder pins, convert binary to decimal
void EncoderState(int &rotate)
{
  rotate = (digitalRead(pinEncoderB) * 2) + digitalRead(pinEncoderA);
}

// Read the encoder and gray code algorithm
int ReadEncoder()
{
  boolean Bpsh = false;
  int Position;
  int isForward = 0;
  String flw = "continue";

  //ResetPins();

  EncoderState(Position); //get initial position of the encoder

  while (!Bpsh)  //loop until change in encoder
  {
    int Position2;
    do    //keep probing the encoder position until different than initial state
    {
      EncoderState(Position2);
      Bpsh = butDebounce(but1p50[0], but1p50[1], but1p50[2], but1p50[3]); //test for cancel
      if (Bpsh == true) {
        flw = "Cancel";
      } else {
        Bpsh = butDebounce(butEp51[0], butEp51[1], butEp51[2], butEp51[3]);
        if (Bpsh == true) {
          flw = "tNext";
        }//endif
      }//endelse
      delay(10); //if not here encoder will spontaneously return exit value
    } while (Position2 == Position && !Bpsh);

    if (flw == "Cancel") {
      return 2;
    }

    if (flw == "tNext") {
      return 3;
    }

    if (Position2 != Position)  //when change detected interpret gray code
    {
      //test forwards or backwards using gray code
      int isFwd = ((Position == 0) && (Position2 == 1)) ||
                  ((Position == 1) && (Position2 == 3)) ||
                  ((Position == 3) && (Position2 == 2)) ||
                  ((Position == 2) && (Position2 == 0));
      int isBwd = ((Position == 2) && (Position2 == 3)) ||
                  ((Position == 3) && (Position2 == 1)) ||
                  ((Position == 1) && (Position2 == 0)) ||
                  ((Position == 0) && (Position2 == 2));

      int tst;

      //interpret change
      //tst == 1 for CW
      //tst == -1 for CCW
      //tst == 0 for ambiguous
      if (isFwd) {
        tst = 1;
      }
      else if (isBwd) {
        tst = -1;
      }
      else {
        tst = 0;
      }

      long tim = millis() - LDT; //time since last encoder change

      //return values with acceptable encoder change
      if (tim > EncoderDel) { //only accept if time elapsed is > than butDeb
        if (tst == 1) {
          LDT = millis(); //reset last delay time
          return 1;       //return encoder change
        }
        else if (tst == -1) {
          LDT = millis();
          return -1;
          }
          else {
            return 0;  //encoder value not acceptable. Restart loop
          }
        return 0;     //redundant?
      }//endif (tim>budeb)

    } //end if (position2!=position)

    Position = Position2;   //save new encoder position position

  } //end while (true)
} //end function ReadEncoder

//
// LabView toggle
//
static void CheckLVtoggle(String SendRoutine) {
  String SerStrCheck = "";
  bool LVthere = true;
  char inCharCheck = ' ';
  SerStrCheck.reserve(500);
  String SerBufCheck = "";
  // Button push detected to go to labview mode
  if (but2p52[2] == 1) {
    Serial1.print("ALmLV");  
    digitalWrite(vM,HIGH); //Labview Mode
    delay(500);
    // Wait for LabView response
    for (int tt = 1; tt <= 120; tt++) {
      // Read the data from LabView
      while (Serial1.available() && inCharCheck != '*') {      //reads commands from Serial (LabVIEW)
        char inCharCheck = (char)Serial1.read();  //reads current character at serial port
        SerBufCheck += inCharCheck;               //appends character to the Serial String
        if (inCharCheck == '*') {
          lcd.setCursor(14, 3);
          lcd.print("      ");
          lcd.setCursor(14, 3);
          lcd.print(SerBufCheck);
          SerStrCheck = SerBufCheck.substring(0, SerBufCheck.length() - 1);
          SerBufCheck = "";
        }
      }  //end string read while
      lcd.setCursor(1, 0);
      lcd.print(SerStrCheck);
      if (SerStrCheck == "LVyes") { //if labview responds send LVthere=true & break
        LVthere = true;
        Cmode = "mLV";
        Serial1.print(SendRoutine);
        break;
      } else {
        LVthere = false;
      }
    }//end for loop timing

    //notify user that labview did not connect. and reset button
    if (LVthere == false) {
      but2p52[2] = 0;
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("ERROR|********|ERROR");
      lcd.setCursor(0, 1); lcd.print("  Labview  timeout");
      lcd.setCursor(0, 2); lcd.print("  Check connection");
      lcd.setCursor(0, 3); lcd.print("    Run  Labview");
      delay(5000);
    }

  } else {
    digitalWrite(vM, LOW); //arduino Mode
    but2p52[2] = 0;
    Cmode = "mAR";
    Serial1.print("ALmAR");    //tell Labview to go to arduino mode
  }
  lcd.setCursor(14, 3);
  lcd.print(Cmode);
  coldedgeDisplay();
  valveDisplay(SystemStatus);

}

//
// Display the pressure (bar) and send to LV
//
void chargeDisplay() {
  String sndString = "";

  lcd.clear();

  // Clear field
  lcd.setCursor(12, 3); lcd.print("       ");

  vlt = analogRead(Pp) * 5.0 / 1024.0; //get voltage
  PSI = vlt * 200 / 5;  //convert to psi

  lcd.setCursor(0, 3); lcd.print("He pressure "); lcd.print(PSI); lcd.print(" PSI");
  sndString = "ALcPR/" + String(PSI) + "*";
  Serial1.print(sndString);

  // Display gauge
  lcd.setCursor(0, 0);
  lcd.print("0");  lcd.print("  ");
  lcd.print("20"); lcd.print("  ");
  lcd.print("40"); lcd.print("  ");
  lcd.print("60"); lcd.print("  ");
  lcd.print("80");

  //  Display vertical bars
  lcd.setCursor(0, 1);
  lcd.write(4); lcd.print(" ");
  lcd.write(4); lcd.print(" ");
  lcd.write(4); lcd.print(" ");
  lcd.write(4); lcd.print(" ");
  lcd.write(4); lcd.print(" ");
  lcd.write(4); lcd.print(" ");
  lcd.write(4); lcd.print(" ");
  lcd.write(4); lcd.print(" ");
  lcd.write(4); lcd.print(" ");
  lcd.write(4);

  // Display pressure reading
  lcd.setCursor(0, 2); lcd.print("                    ");
  lcd.setCursor(0, 2);
  for (int k = 1; k <= PSI / 5; k++) {
    lcd.write(8);
  }
  delay(tChargeUpdate);
}

//
//  Charge 
//
void AutoCharge() {

  chargeDisplay();
  digitalWrite(vC, HIGH); //set He charge status light
  //v1.18 JCP 11/15/18 Change Charge function: Open V2, V7 --> Rr On, Open V7, V3, V4
  digitalWrite(Rr, HIGH); // Recirculator
  digitalWrite(v7, HIGH); // Valve 7
  digitalWrite(v3, HIGH); // Valve 3
  digitalWrite(v4, HIGH); // Valve 4

  while (PSI <= aPressure) {
    chargeDisplay();

  }
  digitalWrite(vC, LOW); //set He charge status light
  //v1.18 JCP 11/15/18 Change Charge function: Open V2, V7 --> Rr On, Open V7, V3, V4
  //v1.20 JCP 11/21/18 Change Charge function: Only close V3 and V7 when button is released
  digitalWrite(v7, LOW); // Valve 7
  digitalWrite(v3, LOW); // Valve 3
}
