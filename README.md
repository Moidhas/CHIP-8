# CHIP-8 Emulator

## Overview
This project is a CHIP-8 emulator written in C++. CHIP-8 is a simple, interpreted programming language designed for running on 8-bit microcomputers. This emulator allows you to load and execute CHIP-8 programs, such as the provided `IBM_LOGO.ch8` executable.

## Project Structure
```
CHIP-8 Emulator
├── build/         # Directory containing the compiled emulator
│   └── chip8      # The runnable CHIP-8 emulator
├── src/           # Source code directory
│   └── main.cc    # Main program file for the emulator
├── Makefile       # Build automation tool
├── IBM_LOGO.ch8   # Example CHIP-8 executable (IBM logo)
└── README.md      # Project documentation
```

## Prerequisites
To build and run the emulator, you will need:
- A C++ compiler that supports C++11 or later
- `make` for building the project

## Building the Emulator
To compile the emulator, navigate to the project root directory and run:
```bash
make
```
This will create a `build/` directory and place the compiled emulator executable (`chip8`) inside it.

## Running the Emulator
After building the emulator, you can run it with the provided `IBM_LOGO.ch8` program or any other compatible CHIP-8 program:
```bash
./build/chip8 IBM_LOGO.ch8
```
Replace `IBM_LOGO.ch8` with the path to your desired CHIP-8 program.

## Features
- **Emulation**: Executes CHIP-8 programs accurately.
- **Example Program**: Includes `IBM_LOGO.ch8`, a CHIP-8 program that displays the IBM logo.
- **Makefile**: Automates the build process.

## Limitations
This emulator supports the standard CHIP-8 instruction set. Extensions or quirks from modern variations (e.g., SCHIP or XO-CHIP) may not be supported.
