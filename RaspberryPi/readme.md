# Readme!

## First time setup

Install the latest version of Raspbian.
Then, open up a Terminal (using GUI or SSH) and run the following commands to get the latest packages:
```sh
sudo apt-get update
sudo apt-get upgrade
```
Once this is done reboot and run:
```sh
sudo nano /boot/config.txt
```
This will allow us to edit the boot config to activate SPI. Edit the following lines in the config.txt file:
```sh
dtparam=spi=on
dtoverlay=mcp2515-can0,oscillator=16000000,interrupt=25
dtoverlay=spi0-hw-cs
```
Another requirement is the can-utils package.
```sh
sudo apt-get install can-utils
```

## Flashing the program on the keys

First you need to import the program you compiled from the computer on to the Raspberry Pi. There's multiple ways to do that, I did it with SSH.
Then you need to cd in the directory of where you put the BootCommander program and run:
```sh
./BootCommander -s=xcp -t=xcp_can -d=can0 ../../key-firmware/example_app.srec -tid=022 -rid=012
```
Where tid and rid correspond to what CAN IDs you wrote when compiling the Bootloaders (more on that in the Keys folder).
I've also added the programmer.py file that flashes the 8 keys in a row to simplify the process.

## Running the MIDI Forwarder

You need to complete first the following line in the key.py file with your computer's local IP address (eg. 192.168.1.13)
```sh
HOST, PORT = "REPLACE_WITH_COMPUTER_LOCAL_IP", 9999
```
Then you can run the key.py file and it will forward the MIDI messages to the computer.
