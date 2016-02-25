# Files in Source Code

These are some of the various source code pieces for the MDRobot project.

The multi-part file, Robot_R4_022116.* is a Codesys v3.5.8 project archive, with all libraries included.

The python script, gpsdData.py, is run from Codesys to utilize the gpsd daemon-supplied gps data.

The sketch_Teensy_MDRobot.ino is the Arduino sketch file for the Teensy to act as an I2C slave to the Codesys Pi.

The folder Robot_R4_pdfs contains all of the project's POUs, FBs, and METHODs used.  Everything is in ST (Structured text). 
**Note: A couple of these are unmodified from the original samples from Codesys and the OSCAT libraries, but are included for clarity.
