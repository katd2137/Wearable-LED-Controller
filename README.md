Hi everyone!

I'm working on a project and could really use some help troubleshooting. I'm building a garment with embedded LED panels, and the setup involves the following. This is my first ever PCB design and electronics

Problem:

Sometimes the LED's are not working (fading on or off). I have rebuilt this board multiple times. When only the very core pins were soldered directly to with jumper cables it all seemed to work perfectly. I have a guess that it could be to do with the grounding, esspecially the way H1 and H2 were designed as ground has to go through 2 x JST connectors and the screw terminals of the regulator. To address this I added a new line in the schematic to connect the ground lines (highlighted in red), as this connection is not on the current PCB but can always solder a jumper to test if people can't spot any other issues. I've since learned about copper fill.
Components:

- ESP8266 Feather Huzzah https://thepihut.com/products/adafruit-feather-huzzah-with-esp8266-wifi
- A 3.7V LiPo battery connected to a Pololu voltage regulator (3.7V to 5V) https://thepihut.com/products/pololu-5v-3a-step-up-step-down-voltage-regulator-s13v30f5
- 5V LEDs
- Custom-printed PCB (schematic and design attached)
- MOSFET irlb8721pbf  and a 10k resistor https://docs.rs-online.com/4bbd/0900766b80f35037.pdf
- 2 buttons with a 10k resistor for each to turn the LEDs on and off manually
- a switch to turn the device off power to the LEDs

Functionality:
- Two buttons, each connected via a 10k resistor to GND, are supposed to trigger a function to turn the LEDs on and off. 
- A master switch controls the LEDs.
- There are 4 JST headers (H3 - H8) for connecting 4 LED panels (currently testing with a single LED strip using crocodile clips).
One of the problems was that the PCB footprint for ESP8266 that we have used had different pin locations from the actual board used (a mistake as I thought it they were all the same across the adafruit feather range). We needed to rewrite the code for the correct PIN numbers from 13 (printed on the pub) to 14 (the actual placement).
JST H1 is the 3.7v Lipo output from the VBAT pin - and feeds into the input side of the step-up regulator. H2 is then for the output of that same regulator, feeding the 5v back into the board.  H9 is the switch.

Here’s what I’ve done so far:

Code: All files and code are uploaded to Github.
Code seems to work, the web app works perfectly and has been shown to work, the serial monitor also shows the mosfet gate pwm value 0-255 as it should.

Testing:
Checked the connectivity of every pin by  wiring the components manually. This was to rule out PCB printing errors.
Checked the battery power at all stages and got the correct voltage (asides from at the led), LED condition working with a bench power supply etc.
Verified that the program uses the correct pin numbers. 

Issue:
Initially, we re-soldered the mosfet, and the code worked.
After testing the buttons, the system stopped functioning again. Current code does not have buttons.

If anyone has experience with a similar setup or can spot potential mistakes, I’d really appreciate your advice! If you have any questions what so every please ask!
Thanks in advance!

Kasia
