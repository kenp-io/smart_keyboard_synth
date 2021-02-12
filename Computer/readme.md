# Readme!

## MIDI virtual instrument

Activate a virtual MIDI connection on your Mac : https://feelyoursound.com/setup-midi-os-x/
You need to complete first the following line in the virtualMIDI.py file with your computer's local IP address (eg. 192.168.1.13)
```sh
HOST, PORT = "REPLACE_WITH_COMPUTER_LOCAL_IP", 9999
```
Then you can run the virtualMIDI.py file and the received MIDI messages from the Raspberry Pi and they will be forwarded to the virtual instrument.

## Hearing the notes

You can use any MIDI compatible DAW. Here I used Ableton Lite. For the purpose to demonstrate per-note pitch bend, I created a track for each MIDI channel on the instrument. Example file is example.als
