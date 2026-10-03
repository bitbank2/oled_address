//
// oled_address
//
// Display the current IP address(es) on the first
// SSD1306 OLED display found on any I2C bus
//
// SPDX-FileCopyrightText: 2026 Larry Bank <bitbank@pobox.com>
// SPDX-License-Identifier: Apache-2.0
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <sys/utsname.h>

#include <OneBitDisplay.h>
ONE_BIT_DISPLAY obd;

//
// Search the given I2C bus for an OLED display
// return: 1=found, 0=none found
//
int I2CFindOLED(int iBus)
{
char szTemp[32];
int iTotal = 0;
int file_i2c;

    snprintf(szTemp, sizeof(szTemp)-1, "/dev/i2c-%d", iBus);
    file_i2c = open(szTemp, O_RDWR);
    if (file_i2c < 0) {
        printf("Failed to open %s\n", szTemp);
        return 0;
    }
    for (int iAddr=0x3c; iAddr <= 0x3d; iAddr++) {
        if (ioctl(file_i2c, I2C_SLAVE, iAddr) >= 0) {
            // Probe this address
            uint8_t ucTemp = 0;
            write(file_i2c, &ucTemp, 1);
            if (read(file_i2c, &ucTemp, 1) >= 0) {
                iTotal++;
                break;
            }
        }
    } // for each address
    close(file_i2c);
    return iTotal;
} /* I2CFindOLED() */
//
// Display the network info on the given I2C bus
//
void ShowAddr(int iBus)
{
struct utsname un;
struct ifaddrs *addrs, *tmp;
int iTimeout = 0;
int bFound = 0;

    obd.setI2CPins(iBus, 0);
    obd.I2Cbegin(OLED_128x64);
    //obd.setContrast(48); // middle brightness to prevent burn-in
    obd.allocBuffer();
    obd.setFont(FONT_8x8);

    uname(&un); // Get the current network name
try_again:
    obd.fillScreen(OBD_WHITE);
    obd.setCursor(0,0);
    obd.println(un.nodename);
    obd.println(" ");
    getifaddrs(&addrs); // Get the IP address(es) of all network interfaces
    tmp = addrs;

    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            if (strncmp(tmp->ifa_name, "lo", 2) != 0) { // we don't need to see "lo"
                obd.print(tmp->ifa_name); obd.println(":");
                obd.println(inet_ntoa(pAddr->sin_addr));
                obd.println(" ");
                bFound = 1;
            } // if not 'lo'
        } // if a network interface
        tmp = tmp->ifa_next; // next in the linked list
    } // while (tmp)
    freeifaddrs(addrs);
    if (!bFound) {
        obd.setCursor(0,0);
        iTimeout++;
        if (iTimeout == 10) {
            obd.print("timed out...    ");
            obd.display(); 
        } else {
            obd.print("waiting...      ");
            obd.display();
            usleep(30*1000000); // wait 30 seconds for DHCP to finish
            goto try_again;
        }
    }
    obd.display(); // write the local framebuffer to the physical display
} /* ShowAddr() */

int main(int argc, char *argv[])
{
int iBus;
DIR *pDir;
struct dirent *pDE;
uint32_t u32Buses = 0; // available I2C bus numbers (0-31)
int iDuration = 5 * 60; // Keep it visible for 5 minutes by default

    if (argc == 2) { // user specified the duration
        iDuration = atoi(argv[1]);
    }

// I2C buses in Linux are defined as a file in the /dev directory
    pDir = opendir("/dev");
    if (!pDir) {
        printf("Error searching /dev directory; aborting.\n");
        printf("Are you running as sudo?\n");
        return -1;
    }
// Search all names in the /dev directory for those starting with i2c-
    while ((pDE = readdir(pDir)) != NULL) {
        if (memcmp(pDE->d_name, "i2c-", 4) == 0) { // found one!
            iBus = atoi(&pDE->d_name[4]);
            u32Buses |= (1 << iBus); // collect the bus numbers
        }
    } // while searching all names in /dev
    closedir(pDir);
    if (u32Buses == 0) {
        printf("No I2C buses found, did you enable I2C?\n");
        return -1;
    }
// Search each I2C bus for a supported proximited sensor
    for (iBus=0; iBus<32; iBus++) {
        if (u32Buses & (1<<iBus)) { // a bus that we found in /dev
            //printf("Searching /dev/i2c-%d...", iBus);
            if (I2CFindOLED(iBus)) { // scan for an OLED
                ShowAddr(iBus);
                break;
            }
        } // I2C bus found
    } // for each possible bus
    // A duration of 0 seconds means to leave the display on forever
    if (iDuration != 0) {
        usleep(iDuration * 1000000); // Show the info for the given amount of time
        obd.setPower(0); // turn off the OLED
    }
    return 0;
} /* main() */
