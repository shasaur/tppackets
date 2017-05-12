TP arduino packet communication library -  Copyright (c) 2017 Tomas Piga

This is a library designed to connect two arduino devices, and allow them to communicate with each other via sending bytes back and forths.

Read the wiki for more information.

To use this library, keep tppackets.c in your folder and include tppackets.h in your main code.

USAGE
-------------
To setup the hardware side of two communicating devices (D1 & D2) you must:
- Connect their ground pins to each other 
  (i.e. pin FG to other device's pin FG)
- Connect D1's P0 to D2's P2
- Connect D1's P1 to D2's P3
- Connect D2's P0 to D1's P0
- Connect D2's P1 to D1's P1
- WARNING: PLEASE USE A 100K RESISTOR WHEN CONNECTING THE LAST 4 
  TO AVOID POTENTIAL DAMAGE TO YOUR DEVICE

To setup the software side:

- simple use the Rios class included to schedule the operating system to check for any incoming messages by calling 'receive_byte()'. The function will return > 0 if any messages have been received, and -1 if none.

To send any messages to the other device, you can send one byte at a time using the 'send_byte(8-bit int)' function.

Both of these are blocking thus far, but you can adjust the time it takes for a message to send by reducing/increasing the TIMEOUT variable in tppackets.h.