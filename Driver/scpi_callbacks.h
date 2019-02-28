#include "coldedge_utilities.h"
#include <Arduino.h>
#include <scpiparser.h>

/*
 * Respond to *IDN?
 */
struct scpi_response* 
identify(struct scpi_parser_context* context, struct scpi_token* command);

/*
 * Get valve status (true = open, false = closed)
 */
struct scpi_response* 
get_valve1(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response* 
get_valve2(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response* 
get_valve3(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response* 
get_valve4(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response* 
get_valve5(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response* 
get_valve7(struct scpi_parser_context* context, struct scpi_token* command);

/*
 * Get the status of recirculator and cryocooler (true = on, false = off)
 */
struct scpi_response* 
get_recirculator(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response* 
get_cryocooler(struct scpi_parser_context* context, struct scpi_token* command);

/*
 * Get pressure in PSI
 */
struct scpi_response* 
get_pressure(struct scpi_parser_context* context, struct scpi_token* command);


/*
 * Set valve status (true = open, false = closed)
 */
struct scpi_response* 
set_valve1(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response*
set_valve2(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response*
set_valve3(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response*
set_valve4(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response*
set_valve5(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response*
set_valve7(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response*
set_recirculator(struct scpi_parser_context* context, struct scpi_token* command);

struct scpi_response*
set_cryocooler(struct scpi_parser_context* context, struct scpi_token* command);
