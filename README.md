# beaglebone-projects
Various projects that utilize low level hardware instructions to interface with leds, speaker output and joystick input on the beaglebone green.

* Project 1 - Access GPIO and LED lights on the BeagleBone. If the joysitck is in the neutral position the program will flash the boards LED light once, if the joystick is pressed up then the LED  will blink 3 times. The program will display a message after it checks the state of the joystick -- if the joystick is pressed up 10 times then the program will exit. A dealy is processed so that the LED lights blinks for a longer duration.

* Project 2 - Displays the total number of prime numbers found (per second) on the beaglebone cape's 14 segment display. Prime numbers are found by incrementing values from 0 and checking if it is a valid prime. A delay can be added by the user in finding new primes by turning the analog to digital dial on the cape. Their is a UDP listener that will listen for commands to allow user interactions:
	* "count"   -- show # primes found.
	* "get 10"  -- display prime # 10.
	* "last 2"  -- display the last 2 primes found.
	* "first 5" -- display the first 5 primes found.
	* "stop"    -- cause the server program to end.

* Project 3 - Produces sounds to the beaglebone capes speaker based on the accelerometer's detected movement - drum machine.

* Project 4 - A driver to flash leds based on input values consisting of characters a-z or A-Z. Characters are translated into morse code values and displayed as flashes to the LED.
 	* This driver also outputs stats to /proc/morse-code
	 * example usage: $ echo 'hello world' | sudo tee /dev/morse-code
	 		  $ sudo cat /dev/morse-code 

* Project 5 -  Baremetal application that uses the watchdog timer,led, joystick and user input to flash leds and display options to the user.
