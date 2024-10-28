#include "SDL/SDL_render.h"
#include "SDL/SDL_video.h"
#include <SDL/SDL.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fstream>
#include <stack>
typedef unsigned char byte;

const int FPS = 60;
const int SPF = 1000 / 60;  
const int PIXEL_SCALE = 10;
const int SCREEN_HEIGHT = 32;
const int SCREEN_WIDTH = 64;

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

void clear(SDL_Renderer *renderer, byte display[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < SCREEN_HEIGHT; ++i) 
        for (int j = 0; j < SCREEN_WIDTH; ++j) 
            display[i][j] = 0; 
}

void draw(byte *V, byte X, byte Y, byte N, unsigned short I, byte display[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    V[0xF] = 0;
    int y = V[Y] % SCREEN_HEIGHT;
    for (unsigned short i = I; i < I + N && y < SCREEN_HEIGHT; ++i) {
        int x = V[X] % SCREEN_WIDTH;
        byte row = MEM[i];
        for (byte mask = 0x80; mask != 0 && x < SCREEN_WIDTH; mask >>= 1) {
            if (row & mask && display[y][x]) 
                V[0xF] = 1; 
            display[y][x] ^= (row & mask);
            ++x;
        }
        ++y;
    }
}

// renders to backbuffer.
void renderFrame(SDL_Renderer *renderer, byte display[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) { 
            if (display[y][x]) {
                SDL_RenderDrawPoint(renderer, x,  y);
            } 
        }
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

    SDL_Window *window = nullptr;  
    SDL_Renderer *renderer = nullptr;
    SDL_Event e;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH * PIXEL_SCALE, SCREEN_HEIGHT * PIXEL_SCALE, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, PIXEL_SCALE, PIXEL_SCALE);
    byte display[SCREEN_HEIGHT][SCREEN_WIDTH] = { 0 };

    bool quit = false;
    unsigned short opcode;

    while (!quit) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

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
                clear(renderer, display);
                std::cout << "clear" << std::endl;
                break;
            case 0x1000:
                PC = NNN - 2;
                std::cout << "jump" << std::endl;
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
                regs[X] = NN;
                std::cout << "set register" << std::endl;
                break;
            case 0x7000:
                regs[X] += NN;
                std::cout << "add to register" << std::endl;
                break;
            case 0x8000:
                break;
            case 0x9000:
                break;
            case 0xA000:
                I = NNN;
                std::cout << "set I" << std::endl;
                break;
            case 0xB000:
                break;
            case 0xC000:
                break;
            case 0xD000:
                draw(regs, X,  Y, N, I, display);
                std::cout << "display" << std::endl;
                break;
            case 0xE000:
                break;
            case 0xF000:
                break;
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                handleKeyboard(e.key.keysym.scancode);
            }
        }

        renderFrame(renderer, display);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;
    SDL_Quit();
}
