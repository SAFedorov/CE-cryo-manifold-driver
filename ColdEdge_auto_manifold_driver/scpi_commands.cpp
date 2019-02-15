
#include <scpiparser.h>
#include <Arduino.h>

/*Initialization of SCPI commands and definitions of callbacks*/

void define_commands(struct scpi_parser_context* ctx){
	struct scpi_command* source;
	struct scpi_command* measure;

	/* First, initialise the parser. */
	scpi_init(ctx);

	/*
	* After initialising the parser, we set up the command tree. 
	* 
	*  *IDN?         	-> Identify
	*  :VALVE<i>		-> Valve<i> open/closed
	*  :RECIrculator	-> Recirculator on/off
	*  :COOLer			-> Cryocooler on/off
	*  :PRESSure?		-> 
	*/
	scpi_register_command(ctx->command_tree, SCPI_CL_SAMELEVEL, "*IDN?", 5, "*IDN?", 5, NULL);
  
	scpi_register_command(ctx->command_tree, SCPI_CL_SAMELEVEL, "RECIRCULATOR?", 13, "RECI?", 5, NULL);
	scpi_register_command(ctx->command_tree, SCPI_CL_SAMELEVEL, "RECIRCULATOR", 12, "RECI", 4, NULL);
  
	scpi_register_command(ctx->command_tree, SCPI_CL_SAMELEVEL, "COOLER?", 7, "COOL?", 5, NULL);
	scpi_register_command(ctx->command_tree, SCPI_CL_SAMELEVEL, "COOLER", 6, "COOL", 4, NULL);
}


/*
 * Respond to *IDN?
 */
scpi_error_t identify(struct scpi_parser_context* context, struct scpi_token* command){
	scpi_free_tokens(command);

	Serial1.println("ColdEdge Stinger Cryostat, Driver version 0.1, OIC v0.2, AutoCool v1.35");
	return SCPI_SUCCESS;
}

/*
 * Respond to a valve status request :VALVE<n>?
 * valve_n is the position in valvestatus array
 */
