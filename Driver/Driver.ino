#include "buttons.h"
#include "pins.h"
#include "scpi_callbacks.h"
#include <Arduino.h>
#include <scpiparser.h>

/* Serial port configuration */
#define COM_TERMINATOR  '\n'
#define COM_BAUD_RATE   9600
#define COM_BUFF_SIZE   256
#define COM_TIMEOUT     50 // ms


/* Global variables */
struct ce_button_list btn_list;
struct scpi_parser_context ctx;

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

  /* Configure buttons */
  btn_list.head = NULL;
  register_button(&btn_list, 45, &toggle_valve1);       // valve 1 open/close
  register_button(&btn_list, 49, &toggle_valve23);      // valve 2 and 3 open/close
  register_button(&btn_list, 17, &toggle_valve4);       // valve 4 open/close
  register_button(&btn_list, 23, &toggle_valve5);       // valve 5 open/close
  register_button(&btn_list, 48, &toggle_valve7);       // valve 7 open/close
  
  register_button(&btn_list, 47, &toggle_recirculator); // recirculator on/off
  register_button(&btn_list, 46, &toggle_cryocooler);   // cryocooler on/off

  register_button(&btn_list, 6, &set_all_off);          // all off


  /* Register scpi commands */
  /* First, initialise the parser. */
  scpi_init(&ctx);

  /*
  * Set up the command tree. Since there are few commands 
  * 
  *  *IDN?          -> Identify
  *  :VALVE<i>      -> Valve<i> open/closed
  *  :RECirculator  -> Recirculator on/off
  *  :COOLer        -> Cryocooler on/off
  *  :PRESsure?     -> read pressure
  */
  scpi_register_command(ctx.command_tree, SCPI_CL_SAMELEVEL, "*IDN?", 5, "*IDN?", 5, &identify);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE1?", 7, "VALVE1?", 7, &get_valve1);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE2?", 7, "VALVE2?", 7, &get_valve2);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE3?", 7, "VALVE3?", 7, &get_valve3);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE4?", 7, "VALVE4?", 7, &get_valve4);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE5?", 7, "VALVE5?", 7, &get_valve5);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE7?", 7, "VALVE7?", 7, &get_valve7);
  
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "RECIRCULATOR?", 13, "REC?", 4, NULL);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "RECIRCULATOR", 12, "REC", 3, NULL);
  
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "COOLER?", 7, "COOL?", 5, NULL);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "COOLER", 6, "COOL", 4, NULL);

  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "PRESSURE?", 9, "PRES?", 5, &get_pressure);
  

  /* Start serial communication */
  Serial1.setTimeout(COM_TIMEOUT);
  Serial1.begin(COM_BAUD_RATE);
}


/*
 * Main function that is executed continuously
 */
void loop() {
  char line_buffer[COM_BUFF_SIZE];
  unsigned char read_length;
  struct scpi_response* response;
  struct scpi_response* tmp_response;

  int resp_cnt;
  
  while(1)
  {
    /* Execute button callbacks */
    execute_buttons(&btn_list);

    /* Execute commands received from the serial port */
    read_length = Serial1.readBytesUntil(COM_TERMINATOR, line_buffer, COM_BUFF_SIZE);
    if(read_length > 0)
    {
      response = scpi_execute(&ctx, line_buffer, read_length);

      /* Count non-empty responses and send reply if cnt>0*/
      tmp_response = response;
      resp_cnt=0;
      while(tmp_response != NULL)
      {
        if(tmp_response->length>0)
        {
          resp_cnt++;
        }
        tmp_response = tmp_response->next;
      }

      if(resp_cnt>0)
      {
        /* Print response strings to the serial port*/
        tmp_response = response;
        while(tmp_response != NULL)
        {
          Serial1.write((const uint8_t*)tmp_response->str, tmp_response->length);
          if(tmp_response->next != NULL)
          {
            Serial1.print(';');
          }
          else
          {
            Serial1.print(COM_TERMINATOR);
          }
          tmp_response = tmp_response->next;
        }
      }
      
      scpi_free_responses(response);
    }
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

/*
 * Read pressure in PSI
 */
double read_pressure()
{
  double p;
  p = analogRead(PRESS_PIN) / 1024.0; //get value relative to full range
  p = p * 200;  //convert to psi
  return p;
}


/* Callbacks */

/*
 * Set all outputs to 'off' state
 */
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
