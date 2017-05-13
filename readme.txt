TP LaFortuna packet communication library -  
Copyright (c) 2017 Tomas Piga

This is a library designed to connect two arduino devices
and give them the ability to talk with each other by using 
their GPIO pins as communication channels to send bytes
between them.

COMPILING
-------------
To use this library, keep tppackets.c along with the subfolders 
included in your main project folder. Then simply include 
tppackets.h in the code of your main runnable.

Once you have written your code using this library, you can use
the makefile included to compile it.

SETTING UP THE HARDWARE
-------------
To setup the hardware side of two communicating devices 
(D1 & D2) you must connect their PORT Fs to each other. 
These can be found in the bottom right corner of the device and 
include doing the following:

- Connect their ground pins to each other 
  (i.e. pin FG to other device's pin FG)
- Connect D1's P0 to D2's P2
- Connect D1's P1 to D2's P3
- Connect D2's P0 to D1's P0
- Connect D2's P1 to D1's P1
-- WARNING: PLEASE USE A 100K RESISTOR WHEN CONNECTING DOING THE
   LAST 4 POINTS TO AVOID POTENTIAL DAMAGE TO YOUR DEVICE
(You can find a diagram of this on the wiki)

USING THE FUNCTIONS
-------------
First use the pins_init() method to setup the pins into their 
correct modes and values.

Sending messages to another device is done by communicating 
single bytes at a time. Do this using the 'send_byte(8-bit int)' 
function.

If there is a byte pending to be received, you can start 
listening for it using the 'receive_byte()' function. In practice 
you can schedule the OS (Rios included) to periodically check for 
this if you don't know when to expect the message. If there any 
such messages, the function will return > 0, and -1 if alternatively
if there are none.

The communication process is done using a protocol based on the
standard handshake, and will block the current thread until the
byte is communicated. The receive and send functions are not 
reentrant because comms have timed deadlines and any interruptions
may stop them from being able to finish in time and therefore cause
the message to transmission failure. However, you can adjust the 
time it takes for a message to send by reducing/increasing the 
TIMEOUT variable in tppackets.h if you want need the time to be 
shorter or prefer it to be longer but more reliable.

If there is ever any problem, eg you are getting constant 
communication failures (which you shouldn't do), you can always use 
the 'reset_pins()' to bring all pins to their default values and
therefore allow for communication to proceed as before.

FURTHER INFORMATION
-------------
You can read the wiki at this link for anything else or some useful
tips and ideas to get you started!
https://secure.ecs.soton.ac.uk/student/wiki/w/COMP2215_1617_TP_Packets