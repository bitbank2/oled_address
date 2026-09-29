# OLED Address
Project started Sept 28th, 2026<br>
Copyright (c) 2026 BitBank Software, Inc.<br>
Written by Larry Bank<br>
bitbank@pobox.com<br>
<br>
![oled_addr](/oled_addr.jpg?raw=true "OLED Address")
<br>
## What is it?
This project displays the current network name and address on the first I2C OLED display that it finds on the available I2C buses. It will quietly search all I2C buses for a matching address of 0x3c or 0x3d.<br>

## Why did you write it?
I realize that this is an old idea, but I wanted to make it much simpler and more useful than existing projects I've seen. This project is written in native C++ code with no external dependencies (besides my OneBitDisplay library included as a submodule).<br>

## What's Special about it?
The main improvement over other versions of this idea is that it can automatically find the attached SSD1306 OLED by searching available I2C buses. This is especially useful on the many random Linux SBCs which map I2C buses differently than the Raspberry Pi. For example, the OrangePi Zero 2W uses I2C bus number 3 in the location of bus number 1 on Raspberry Pi SBCs. My version also turns itself off after 5 minutes of displaying info to not burn the image into the OLED display. You can change this behavior by passing the number of seconds you'd like on the command line or 0 for never turn off.<br>

## Getting Started
Run the install.sh bash script to build and install the program. It will install libgpiod, make, and build-essential (if needed) and compile plus install the program.<br>

