#include <Arduino.h>

typedef void(*button_callback_t)(void);

struct ce_button
{
	int pin;
	int current_state;
	int previous_state;
	
	button_callback_t callback;
	
	struct ce_button* next;
};

struct ce_button_list
{
	struct ce_button* head;
	struct ce_button* tail;
};

/**
 * Add a button to a list.
 */
struct ce_button*
register_button(struct ce_button_list* button_list, int pin, button_callback_t callback);


/**
 * Add a button to a list.
 */
void
execute_buttons(struct ce_button_list* button_list);

