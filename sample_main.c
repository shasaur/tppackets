/*
File: sample_main.c

 Copyright (c) 2017 Tomas Piga

 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this list
 of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or other
 materials provided with the distribution.

 Neither the name of the Kustaa Nyholm or SpareTimeLabs nor the names of its
 contributors may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 OF SUCH DAMAGE.

 ----------------------------------------------------------------------

This file is a runnable which demonstrates the use of the tppackets library.

It demonstrates how button presses can be sent across to another connected LaFortuna 
device and an example of how to receive them using the Rios library included.

12/05/2017
*/

#include "os.h"
#include "tppackets.h"

// Schedulable functions
int check_switches(int);
int check_for_messages(int);

// Initialisation of all systems, scheduling, and main loop
void main(void) {
    os_init();
	display_string("OS Initialised\n");
	
	pins_init();
	display_string("Inputs & Outputs Initialised\n");

    os_add_task( check_switches,  100, 1);
    os_add_task( check_for_messages,  500, 1);
	display_string("Tasks Scheduled\n");
    
    sei(); // - enable global interrupts after initialisation is finished
    for(;;){}
}

/* 	This shows how a simple message code can be set up
	to interpret any messages received. Specifically, numbers
	0-4 are used to uniquely identify each of the buttons. */
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
	
	// If the wrong code was received
	return "unknown key";
}

/* 	Following functions include sending a different code
	for every type of click and determining whether the message's
	transmission was unsuccessful - note -1 is the code for this */
	
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

/* 	Scheduled function which checks whether any of the
	buttons on the LaFortuna have been clicked and then
	if any have, sends a message to the other device to
	inform of this */
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

/* 	Scheduled function to check whether there is a message
	that that is currently waiting to be sent to this device,
	if so, receives it and interprets it. If the message fails
	in receiving (for which the code is < 0), an error is printed */
int check_for_messages(int state){
	
	// Check if there is an input signal
	int16_t byte = receive_byte();
	
	// If something was received
	if (byte >= 0)
		tfp_printf("\n\t You pressed %s \n", translate_packet(byte));
	
	return state;
}