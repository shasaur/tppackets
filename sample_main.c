/*
	COMP2215 15/16 Task 7
	based off task 5 answer
	by Tomas Piga (tp6g15) 
*/

#include "os.h"
#include "tppackets.h"

int check_switches(int);
int check_for_messages(int);

void main(void) {
    os_init();
	display_string("OS Initialised\n");
	
	pins_init();
	display_string("Inputs & Outputs Initialised\n");

    os_add_task( check_switches,  100, 1);
    os_add_task( check_for_messages,  500, 1);
	display_string("Tasks Scheduled\n");
     
    sei();
    for(;;){
		
	}
    
}

char* translate_packet(uint8_t byte){
	tfp_printf("\Byte received of value %d \t", byte);
	
	switch (byte){
		case 0:
		return "middle";
		break;
		case 1:
		return "left";
		break;
		case 2:
		return "up";
		break;
		case 3:
		return "right";
		break;
		case 4:
		return "down";
		break;
	}
	
	return "unknown key";
}

int click_centre(){
	display_string("Sending input\n");
	
	uint8_t was_successful = send_byte(0);
	if (was_successful == -1)
		display_string("Send failed\n");
	
	return 0;
}

int click_left(){
	display_string("Sending input\n");
	
	uint8_t was_successful = send_byte(1);
	if (was_successful == -1)
		display_string("Send failed\n");
	
	return 0;
}

int click_up(){
	display_string("Sending input\n");
	
	uint8_t was_successful = send_byte(2);
	if (was_successful == -1)
		display_string("Send failed\n");
	
	return 0;
}

int click_right(){
	display_string("Sending input\n");
	
	uint8_t was_successful = send_byte(3);
	if (was_successful == -1)
		display_string("Send failed\n");
	
	return 0;
}

int click_down(){
	display_string("Sending input\n");
	
	uint8_t was_successful = send_byte(4);
	if (was_successful == -1)
		display_string("Send failed\n");
	
	return 0;
}

int check_switches(int state) {
	
	if (get_switch_press(_BV(SWN))) {
		click_up();
	}
		
	if (get_switch_press(_BV(SWE))) {
		click_right();
	}
		
	if (get_switch_press(_BV(SWS))) {
		click_down();
	}
		
	if (get_switch_press(_BV(SWW))) {
		click_left();
	}
		
	if (get_switch_press(_BV(SWC))) {
		click_centre();
	}

	return state;	
}

int check_for_messages(int state){
	
	// Check if there is an input signal
	int16_t byte = receive_byte();
	
	// If something was received
	if (byte >= 0)
		tfp_printf("\n\t You pressed %s \n", translate_packet(byte));
	
	return state;
}