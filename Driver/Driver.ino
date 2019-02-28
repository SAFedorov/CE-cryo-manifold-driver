#include "buttons.h"
#include "coldedge_utilities.h"
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
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE1?", 7, "V1?", 3, &get_valve1);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE2?", 7, "V2?", 3, &get_valve2);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE3?", 7, "V3?", 3, &get_valve3);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE4?", 7, "V4?", 3, &get_valve4);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE5?", 7, "V5?", 3, &get_valve5);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE7?", 7, "V7?", 3, &get_valve7);

  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE1", 6, "V1", 2, &set_valve1);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE2", 6, "V2", 2, &set_valve2);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE3", 6, "V3", 2, &set_valve3);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE4", 6, "V4", 2, &set_valve4);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE5", 6, "V5", 2, &set_valve5);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "VALVE7", 6, "V7", 2, &set_valve7);
  
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "RECIRCULATOR?", 13, "REC?", 4, &get_recirculator);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "RECIRCULATOR", 12, "REC", 3, &set_recirculator);
  
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "COOLER?", 7, "COOL?", 5, &get_cryocooler);
  scpi_register_command(ctx.command_tree, SCPI_CL_CHILD, "COOLER", 6, "COOL", 4, &set_cryocooler);

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
  
  while(1)
  {
    /* Execute button callbacks */
    execute_buttons(&btn_list);

    /* Execute commands received from the serial port */
    read_length = Serial1.readBytesUntil(COM_TERMINATOR, line_buffer, COM_BUFF_SIZE);
    scpi_execute(&ctx, line_buffer, read_length, &commf, COM_TERMINATOR);
  }
}

/*
 * Communicate using serial port
 */
void
commf(char* str, int length)
{
  Serial1.write((const uint8_t*)str, length);
}

/* Button callbacks */

/*
 * Toggle valves
 */
void toggle_valve1(){
  toggle_pin(VALVE1_PIN);
}

void toggle_valve23(){
  toggle_pin(VALVE2_PIN);
  toggle_pin(VALVE3_PIN);
}

void toggle_valve4(){
  toggle_pin(VALVE4_PIN);
}

void toggle_valve5(){
  toggle_pin(VALVE5_PIN);
}

void toggle_valve7(){
  toggle_pin(VALVE7_PIN);
}

void toggle_recirculator(){
  toggle_pin(RECIRC_PIN);
}

void toggle_cryocooler(){
  toggle_pin(COOLER_PIN);
}
