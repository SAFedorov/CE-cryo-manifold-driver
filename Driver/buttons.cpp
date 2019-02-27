#include <Arduino.h>
#include "buttons.h"

#ifdef __cplusplus

extern "C" {
	
#endif

void
execute_buttons(struct ce_button_list* btn_list)
{
	struct ce_button* current_button;
	
	current_button = btn_list->head;
	while(current_button != NULL)
	{
		/* Update state */
		current_button->previous_state = current_button->current_state;
		current_button->current_state = digitalRead(current_button->pin);
		
		/* Execute callback if button was released */
		if(current_button->current_state == 1 && current_button->previous_state == 0 
			&& current_button->callback != NULL)
		{
			current_button->callback(current_button);
		}
		
		current_button = current_button->next;
	}
}


struct ce_button*
register_button(struct ce_button_list* btn_list, int pin, button_callback_t callback)
{
	struct ce_button* new_button;
	
	new_button = (struct ce_button*)malloc(sizeof(struct ce_button));
	new_button->next = NULL;
	new_button->pin = pin;
	new_button->callback = callback;
	
	if(btn_list->head == NULL || btn_list->tail == NULL)
	{
		btn_list->head = new_button;
		btn_list->tail = btn_list->head;
	}
	else
	{
		btn_list->tail->next = new_button;
		btn_list->tail = btn_list->tail->next;
	}
	
	return new_button;
}

void
init_buttons(struct ce_button_list* btn_list)
{
	struct ce_button* current_button;
	
	current_button = btn_list->head;
	while(current_button != NULL)
	{
		pinMode(current_button->pin, INPUT_PULLUP);
	}
}


#ifdef __cplusplus

}

#endif