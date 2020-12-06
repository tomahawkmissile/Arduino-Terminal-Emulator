# Arduino UART Terminal Emulator
### For Arduino MEGA and a TFT screen

### Libraries used: Adafruit GFX, MCUFRIEND_kbv

This program reads any data from the USB serial port and outputs it as text on a TFT display. Currently, it does not support scrolling, so when the screen fills up, it reset the screen to all black and starts printing at back at the top.