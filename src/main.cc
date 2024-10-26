#include <SDL/SDL.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fstream>
#include <stack>
typedef unsigned char byte;
typedef bool bit;

const int FPS = 60;
const int SPF = 1000 / 60;  
const int PIXEL_SCALE = 10;
const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;

byte MEM[4096];
std::stack<unsigned short> return_addrs;

// Special Registers
unsigned short I;
unsigned short PC = 0x200;
byte delay_timer;
byte sound_timer;

// Registers
byte regs[16];

void handleKeyboard(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_1:
            break;
        case SDL_SCANCODE_2:
            break;
        case SDL_SCANCODE_3:
            break;
        case SDL_SCANCODE_4:
            break;
        case SDL_SCANCODE_Q:
            break;
        case SDL_SCANCODE_W:
            break;
        case SDL_SCANCODE_E:
            break;
        case SDL_SCANCODE_R:
            break;
        case SDL_SCANCODE_A:
            break;
        case SDL_SCANCODE_S:
            break;
        case SDL_SCANCODE_D:
            break;
        case SDL_SCANCODE_F:
            break;
        case SDL_SCANCODE_Z:
            break;
        case SDL_SCANCODE_X:
            break;
        case SDL_SCANCODE_C:
            break;
        case SDL_SCANCODE_V:
            break;
        default:
            break;
    }
}
// remember to add error checking where needed (i.e. SDL_INIT)
int main(int argc, char *argv[]) {
    if (argc != 2) return -1;
    std::ifstream program{argv[1], std::ifstream::binary};

    if (!program.is_open()) {
        std::cout << "UNABLE TO OPEN FILE" << std::endl; 
        return -1;
    }

    program.seekg(0, program.end);
    int size = program.tellg();
    program.seekg(0, program.beg);
    program.read((char * ) (MEM + PC), size);
    program.close();

    const byte FONT_SET[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    memcpy(MEM + 80, FONT_SET, sizeof(FONT_SET));

    bool quit = false;
    unsigned short opcode;

    while (!quit) {
        opcode = (MEM[PC] << 8) | MEM[PC + 1];
        std::cout << PC << ": ";
        PC += 2;

        unsigned short NNN = opcode & 0x0FFF;
        byte X = (opcode & 0x0F00) >> 8;
        byte Y = (opcode & 0x00F0) >> 4;
        byte N = opcode & 0x000F;
        byte NN = opcode & 0x00FF;

        switch(opcode & 0xF000) {
            case 0x0000:
                std::cout << "clear" << std::endl;
                break;
            case 0x1000:
                std::cout << "jump" << std::endl;
                PC = NNN - 2;
                break;
            case 0x2000:
                break;
            case 0x3000:
                break;
            case 0x4000:
                break;
            case 0x5000:
                break;
            case 0x6000:
                std::cout << "set register" << std::endl;
                regs[X] = NN;
                break;
            case 0x7000:
                std::cout << "add to register" << std::endl;
                regs[X] += NN;
                break;
            case 0x8000:
                break;
            case 0x9000:
                break;
            case 0xA000:
                std::cout << "set I" << std::endl;
                I = NNN;
                break;
            case 0xB000:
                break;
            case 0xC000:
                break;
            case 0xD000:
                std::cout << "draw" << std::endl;
                break;
            case 0xE000:
                break;
            case 0xF000:
                break;
        }

        SDL_Delay(1000);
    }
}
