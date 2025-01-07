# Flipper Zero Spoof Temp

A modified version of [spoof_temp](https://github.com/rgerganov/spoof_temp) designed specifically for the Flipper Zero. This project allows you to spoof temperature readings directly on the Flipper Zero device, enabling easy testing and experimenting with temperature-related functionality on your device.

## 🚀 Features

- **Flipper Zero Integration**: **ONLY** compatible with the Flipper Zero SUBGHZ system.
- **Temperature Spoofing**: Simulate custom temperature readings.
- **Easy to Use**: Directly interacts with the Flipper Zero interface for seamless usage.
- **Customizable**: Modify the spoofed temperature values to fit your needs for testing or development.

## 📦 Installation

To install the program, follow the steps below:

### 1. Clone the repository:

### 2. Do those steps!
1. sudo apt install libhackrf-dev
2. make
3. ./spoof_temp -t 12.3 -h 40 -o {FileName}

It automatically saves you the .sub file!
Just drag and drop it into the Flipper SD-Card on /subghz/ !
To run it --> open Sub-GHz App on your Flipper --> saved --> there you have it!
