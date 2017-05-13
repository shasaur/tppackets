/*
File: tppackets.c

 Copyright (c) 2017 Tomas Piga

 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this list
 of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or other
 materials provided with the distribution.

 Neither the name of Tomas Piga nor the names of its contributors may be used to 
 endorse or promote products derived from this software without specific prior 
 written permission.

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

To see more information on these classes and their functionality, read the readme.txt
or visit the wikipage/git repo.

 12/05/2017
*/

#include "tppackets.h"

// Checks if any of the pins are on, and turns them off
void reset_pins(){
	if ((PINF & _BV(PINF0)) != 0)
		PINF |= _BV(PF0);
		
	if ((PINF & _BV(PINF1)) != 0)
		PINF |= _BV(PF1);
}

// Breaks down an integer into its constituent bits
uint8_t *get_bits(uint8_t n, uint8_t bitswanted){
  uint8_t *bits = malloc(sizeof(uint8_t) * bitswanted);

  uint8_t k;
  for(k=0; k<bitswanted; k++){
    uint8_t mask =  1 << k;
    uint8_t masked_n = n & mask;
    uint8_t thebit = masked_n >> k;
    bits[k] = thebit;
  }

  return bits;
}

// Checks whether the pin receiving voltage is correct, and communication can progress
int is_expecting(int expectation){
	if ((PINF & _BV(PINF2)) != 0 && expectation == 1)
		return 0;
	
	if ((PINF & _BV(PINF2)) == 0 && expectation == 0)
		return 0;
	
	return 1;
}

// Initialise pins
int8_t pins_init(){
	DDRF = 0;
	PINF = 0;
	
	DDRF  |= _BV(PF0);   /* set everything as input except for output pin 0 */
    PINF &= ~_BV(PF0);   /* off */
	
	DDRF  |= _BV(PF1);   /* pin 1 out */
    PINF &= ~_BV(PF1);   /* off */
	
	// because sometimes the pins seem to stay turned on anyway
	reset_pins();
	
	DDRF  &= ~_BV(PF2);   /* pin 2 input */
    PINF &= ~_BV(PF2);   /* off */
	
	DDRF  &= ~_BV(PF3);   /* pin 3 input */
    PINF &= ~_BV(PF3);   /* off */
	
	return 0;
}

/* 	This takes the integer received, breaks into down into its bits and sends those
	one at a time through the pins to the other device.
	It is uninterruptable so that the signals sent between devices are correctly
	captured within the time frame they are needed in.
*/
uint8_t send_byte(uint8_t num){
	cli();
	
	uint8_t* bit_array = get_bits(num, BYTE);
	
	int successful = start_sending(bit_array);
	
	/*if (i!=-1)
		display_string("Message Sent\n");
	else
		display_string("Message Failed to Send!\n");*/
	
	reset_pins();
	
	sei();
	
	return successful;
}

// This treats each of the numbers in the array as bits, and cycles through them to send them
int start_sending(uint8_t bv[]){
	
	// signal start of comm
	PINF |= _BV(PINF1);
	
	
	// while haven't received acknowledgement
	int ready = 0;
	int expecting = 1; // where 1 is the high acknowledge of connectivity signal from receiver
	int attempts = 100;
	
	//poll for acknowledgement 100 times
	while (is_expecting(expecting) != 0 && attempts > 0) {}
	expecting = 0;

	//tfp_printf("\n\t Attempts: %d \n", attempts);
	_delay_ms(10);
	
	// if acknowledgement was received start sending
	if ( attempts > 0 ) {
		
		// for all of the bits
		int index = 0;
		for (index=0; index<=BYTE; index++){
			
			// If sending a 1
			if (index == BYTE || bv[index]==1 ){
				
				// set bit
				PINF |= _BV(PINF0);
				
				// set sending flag
				PINF |= _BV(PINF1);
				
				// Halt until the expected signal appears, then reset and continue
				int counter = 100;
				while (is_expecting(expecting) != 0 && counter>0) {counter--; _delay_ms(5);}
				if (counter==0) return -1;
				if (expecting==0) expecting = 1; else expecting = 0;
				
				// set reset
				PINF |= _BV(PINF0);
				if (index+1 <= BYTE)
					PINF |= _BV(PINF1);
				
			} else {
				// set sending flag
				PINF |= _BV(PINF1);
				
				// Halt until the expected signal appears, then reset and continue
				int counter = 100;
				while (is_expecting(expecting) != 0 && counter>0) {counter--; _delay_ms(5);}
				if (counter==0) return -1;
				if (expecting==0) expecting = 1; else expecting = 0;
				
				// set reset
				if (index+1 <= BYTE)
					PINF |= _BV(PINF1);
			}
			
			/*if (index < BYTE)
				tfp_printf("\n\t Pin %d: %d \n", index, bv[index]);
			else
				tfp_printf("\n\t Error bit: 1\n");*/
			_delay_ms(12);
		}
		
		return 0;
	
	// If failed to send
	} else {
		return -1;
	}
}

/* 	Upon receiving a control signal, communication will initiate with the
	other device and this will listen for any incoming bits, returning them
	as a number */
int8_t start_receiving(){
	
	uint8_t packet[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint8_t curr_bit = 0;
	
	// send confirmation
	PINF |= _BV(PINF0); 
		_delay_ms(10);
	
	// start receiving
	int index=0;
	int timeout=TIMEOUT/2;
	int last_high = 1;
	for (index=0; index<=BYTE && timeout>0;timeout--){
		
		// If receiving signal is high
		if ( (PINF & _BV(PINF3)) == 0 && last_high == 1) {
			
			// Record the received bit
			if ( (PINF & _BV(PINF2)) != 0 )
				packet[curr_bit++] = 1;
			else
				packet[curr_bit++] = 0;
			
			index++;
			last_high = 0;
			
			
			_delay_ms(10);
			PINF |= _BV(PINF0); 
			
			//tfp_printf("Received bit: %d\n", packet[curr_bit-1]);
			
		// If receiving signal is back low, reset to be able to start receiving again
		} else if ((PINF & _BV(PINF3)) != 0) {
			last_high = 1;
		}
		_delay_ms(10);
	}
	
	// Check error bit
	if (packet[BYTE]==0)
		return -1;
	
	uint8_t byte = packet[0]<<0 | packet[1]<<1 | packet[2]<<2 | packet[3]<<3
				 | packet[4]<<4 | packet[5]<<5 | packet[6]<<6 | packet[7]<<7;
				 
	reset_pins();
	
	return byte;
}

/* 	Returns -1 if nothing was received, and -2 if there was an error
	It is uninterruptable so that the signals sent between devices are correctly
	captured within the time frame they are needed in.
*/
int16_t receive_byte(){
	cli();
	
	// If there is a message waiting
	if ( (PINF & _BV(PINF3)) != 0) {
		int message = start_receiving();
		if (message == -1)
			return -2;
		else
			return message;
	
	} else
		return -1;
	
	sei();
}