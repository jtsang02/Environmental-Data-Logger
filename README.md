# Environmental-Data-Logger

The environmental data logger reads data from the potentiometer, photo resistor, and TMP36 temperature sensor (inputs) to continuously store data taken every 10 seconds.  The data is displayed on both the serial monitor and LCD (outputs). The setting of the dip switch determines whether data is displayed every 1 minute or 10 minutes on the serial monitor, and the push button toggles which data is displayed on the LCD.  The serial monitor displays location, time, wind, light, and temperature while the LCD displays only one of wind, light or temperature.  The following diagram illustrates the various inputs and outputs read from the Arduino. 

Hardware Required: (Arduino Mega Kit) 

1. LEDs with Current Limiting Resistor (470 ohm) as needed1 Red, 1 Yellow, and 1 Green LED + an optional 1 Tri-colour LED
2. DIP Switch Inputs as  needed
3. 1 - 10 kOhm potentiometer
4. 1 - Photo-resistor
5. 1 - TMP36 Temperature sensor
6. 1 - LCD (In the first stage of this lab, you will work with the serial monitor)
7. 1 - Push button


Hardware Setup

![image](https://user-images.githubusercontent.com/77314661/109382174-aba25400-7893-11eb-81b8-5a67fb8afeef.png)


The data logger has various input and output sources and uses both analog and digital reads on the Arduino.  The built in <TimerThree.h> library is used to blink the red LED when the windspeed is read as gusty or windy, in order to not interrupt the loop function that is reading data continuously.  The LCD uses the <SoftwareSerial.h> library.

Data for each minute is taken by taking data recorded from the potentiometer, TMP36 and photo resistor every 10 seconds using the delay() function, totalling these numbers up and averaging over 6 readings.  The value after a minute is recorded and displayed on the serial monitor if the switch reads high in test mode.  If switch reads low, the average data over 10 minutes is displayed. 

The button to change settings on the LCD uses attach.interrupt() function to monitor when the button is pressed while the program is running. 

Various other functions are used to execute specific tasks throughout the program such as printing the header for serial monitor, clearing the LCD display, blink LED, or converting voltage readings of the pins to actual measurements.  
