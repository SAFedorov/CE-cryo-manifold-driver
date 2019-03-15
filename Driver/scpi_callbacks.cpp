#include "coldedge_utilities.h"
#include <Arduino.h>
#include <scpiparser.h>

/* Get commands */

struct scpi_response* 
identify(struct scpi_parser_context* context, struct scpi_token* command)
{
	struct scpi_response* resp;

	resp = get_empty_response(40);
	resp->length = sprintf(resp->str, "%s", "EPFL driver for ColdEdge cryo v1.0");
	resp->length--; // discard the EOS character
  
	return resp;
}

static struct scpi_response* 
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
	return get_pin_stat(VALVE1_PIN);
}

struct scpi_response* 
get_valve2(struct scpi_parser_context* context, struct scpi_token* command)
{
	return get_pin_stat(VALVE2_PIN);
}

struct scpi_response* 
get_valve3(struct scpi_parser_context* context, struct scpi_token* command)
{
	return get_pin_stat(VALVE3_PIN);
}

struct scpi_response* 
get_valve4(struct scpi_parser_context* context, struct scpi_token* command)
{
	return get_pin_stat(VALVE4_PIN);
}

struct scpi_response* 
get_valve5(struct scpi_parser_context* context, struct scpi_token* command)
{
	return get_pin_stat(VALVE5_PIN);
}

struct scpi_response* 
get_valve7(struct scpi_parser_context* context, struct scpi_token* command)
{
	return get_pin_stat(VALVE7_PIN);
}

struct scpi_response*
get_recirculator(struct scpi_parser_context* context, struct scpi_token* command)
{
	return get_pin_stat(RECIRC_PIN);
}

struct scpi_response*
get_cryocooler(struct scpi_parser_context* context, struct scpi_token* command)
{
	return get_pin_stat(COOLER_PIN);
}

struct scpi_response*
get_pressure(struct scpi_parser_context* context, struct scpi_token* command)
{
	struct scpi_response* resp;
	double p;
	int i;

	p = read_pressure();

	resp = get_empty_response(10);
	dtostrf(p, 1, 2, resp->str);

	/* Count the printed symbols excluding EOS*/
	for(i=0; resp->str[i] != '\0' && i<10; i++);
	resp->length = i-1;

	return resp;
}


/* Set commands */

static struct scpi_response*
set_pin_stat(int pin, struct scpi_token* command)
{
	struct scpi_token* arg;
	struct scpi_numeric out_numeric;
	
	arg = command;
	while(arg != NULL && arg->type != SCPI_CT_ARG)
	{
		arg = arg->next;
	}
	
	if(arg != NULL)
	{
		out_numeric = scpi_parse_numeric(arg->value, arg->length, 0, 0, 1);
		digitalWrite(pin, out_numeric.value);
	}

	return get_empty_response(0);
}

struct scpi_response*
set_valve1(struct scpi_parser_context* context, struct scpi_token* command)
{
	return set_pin_stat(VALVE1_PIN, command);
}

struct scpi_response*
set_valve2(struct scpi_parser_context* context, struct scpi_token* command)
{
	return set_pin_stat(VALVE2_PIN, command);
}

struct scpi_response*
set_valve3(struct scpi_parser_context* context, struct scpi_token* command)
{
	return set_pin_stat(VALVE3_PIN, command);
}

struct scpi_response*
set_valve4(struct scpi_parser_context* context, struct scpi_token* command)
{
	return set_pin_stat(VALVE4_PIN, command);
}

struct scpi_response*
set_valve5(struct scpi_parser_context* context, struct scpi_token* command)
{
	return set_pin_stat(VALVE5_PIN, command);
}

struct scpi_response*
set_valve7(struct scpi_parser_context* context, struct scpi_token* command)
{
	return set_pin_stat(VALVE7_PIN, command);
}

struct scpi_response*
set_recirculator(struct scpi_parser_context* context, struct scpi_token* command)
{
	return set_pin_stat(RECIRC_PIN, command);
}

struct scpi_response*
set_cryocooler(struct scpi_parser_context* context, struct scpi_token* command)
{
	return set_pin_stat(COOLER_PIN, command);
}
