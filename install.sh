#!/bin/bash
# This script builds and installs the oled_address program
sudo apt install libgpiod-dev make build-essential -y
cd OneBitDisplay/Linux
make
cd ../..
make
sudo chmod 4755 oled_addr
sudo cp oled_addr /usr/local/bin
echo "oled_addr is now built and installed in /usr/local/bin"
echo "To have it run at startup, edit /etc/rc.local and add the line:"
echo "/usr/local/bin/oled_addr BEFORE the line with 'exit 0'."
echo "If you want to change the default timeout (5 minutes), add"
echo "the number of seconds to stay on as a parameter after oled_addr"
echo "e.g. /usr/local/bin/oled_addr 60"
echo "to keep the information visible for 60 seconds"

