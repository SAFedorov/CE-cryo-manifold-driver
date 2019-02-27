#include "pins.h"
#include <Arduino.h>
#include <scpiparser.h>

/*
 * Respond to *IDN?
 */
struct scpi_response* 
identify(struct scpi_parser_context* context, struct scpi_token* command)
{
	struct scpi_response* resp;
  
	scpi_free_tokens(command);

	resp = get_empty_response(20);
	strcpy(resp->str, "CE Cryo driver v0.1");
	resp->length--; // discard the EOS character
  
	return resp;
}

/*
 * Get valves status
 */

struct scpi_response* 
get_pin_stat(int pin)
{
	struct scpi_response* resp;
	int stat;
	stat = digitalRead(pin);
	resp = get_empty_response(2);
	resp->length = sprintf(resp->str, "%i", stat);
	return resp;
}

struct scpi_response* 
get_valve1(struct scpi_parser_context* context, struct scpi_token* command)
{
	scpi_free_tokens(command);
	return get_pin_stat(VALVE1_PIN);
}

struct scpi_response* 
get_valve2(struct scpi_parser_context* context, struct scpi_token* command)
{
	scpi_free_tokens(command);
	return get_pin_stat(VALVE2_PIN);
}

struct scpi_response* 
get_valve3(struct scpi_parser_context* context, struct scpi_token* command)
{
	scpi_free_tokens(command);
	return get_pin_stat(VALVE3_PIN);
}

struct scpi_response* 
get_valve4(struct scpi_parser_context* context, struct scpi_token* command)
{
	scpi_free_tokens(command);
	return get_pin_stat(VALVE4_PIN);
}

struct scpi_response* 
get_valve5(struct scpi_parser_context* context, struct scpi_token* command)
{
	scpi_free_tokens(command);
	return get_pin_stat(VALVE5_PIN);
}

struct scpi_response* 
get_valve7(struct scpi_parser_context* context, struct scpi_token* command)
{
	scpi_free_tokens(command);
	return get_pin_stat(VALVE7_PIN);
}

struct scpi_response*
get_pressure(struct scpi_parser_context* context, struct scpi_token* command)
{
	struct scpi_response* resp;
	double p;

	scpi_free_tokens(command);

	p = analogRead(PRESS_PIN) / 1024.0; //get value relative to full range
	p = p * 200;  //convert to psi

	resp = get_empty_response(10);
	resp->length = sprintf(resp->str, "%i", p);
	return resp;
}