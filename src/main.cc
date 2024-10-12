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
unsigned short PC = 512;
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
    std::ifstream program(argv[1], std::ifstream::binary);

    if (!program.is_open()) {
        std::cout << "UNABLE TO OPEN FILE" << std::endl; 
        return -1;
    }

    program.seekg(0, program.end);
    int length = program.tellg();
    program.seekg(0, program.beg);
    program.read((char * ) (MEM + PC), length);

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

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH * PIXEL_SCALE, SCREEN_HEIGHT * PIXEL_SCALE, 0, &window, &renderer);
    SDL_RenderSetScale(renderer, PIXEL_SCALE, PIXEL_SCALE);
    byte display[SCREEN_WIDTH][SCREEN_HEIGHT] = { 0 };
    

    srand((unsigned) time(NULL));    

    // SDL_Surface *surface = SDL_GetWindowSurface(window);
    // SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
    // SDL_UpdateWindowSurface(window);
    SDL_Event e;
    bool quit = false;
    unsigned short opcode;

    while (!quit) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        Uint32 startTime = SDL_GetTicks();
        delay_timer -= delay_timer > 0 ? 1 : 0;
        sound_timer -= sound_timer > 0 ? 1 : 0;

        memcpy(&opcode, MEM + PC, 2);
        PC += 2;
        byte nibble = opcode & 0x000F;
        byte X = (opcode & 0x00F0) >> 4;
        byte Y = (opcode & 0x0F00) >> 8;
        byte N = (opcode & 0xF000) >> 12;
        byte NN = (opcode & 0xFF00) >> 8;
        byte NNN = (opcode & 0xFFF0) >> 4;

        switch(nibble) {
            case 0x0:
                switch(NNN) {
                    case 0x0E0:
                        std::cout << "CLEAR" << std::endl;
                        for (int i = 0; i < SCREEN_WIDTH; ++i) {
                            for (int j = 0; j < SCREEN_HEIGHT; ++j) {
                                display[i][j] = 0;
                            }
                        }
                        break;
                    case 0x0EE:
                        std::cout << "RETURN" << std::endl;
                        PC = return_addrs.top();
                        return_addrs.pop();
                        break;
                }
                break;
            case 0x1:
                std::cout << "JUMP" << std::endl;
                PC = NNN;
                break;
            case 0x2:
                std::cout << "CALL function" << std::endl;
                return_addrs.push(PC);
                PC = NNN;
                break;
            case 0x3:
                std::cout << "SKIP" << std::endl;
                if (regs[X] == NN) PC += 2; 
                break;
            case 0x4:
                std::cout << "SKIP" << std::endl;
                if (regs[X] != NN) PC += 2; 
                break;
            case 0x5:
                std::cout << "SKIP" << std::endl;
                if (regs[X] == regs[Y]) PC += 2;
                break;
            case 0x6:
                std::cout << "SET REG" << std::endl;
                regs[X] = NN;
                break;
            case 0x7:
                std::cout << "ADD TO REG" << std::endl;
                regs[X] += NN;
                break;
            case 0x8:
                switch(N) {
                    case 0x0:
                        regs[X] = regs[Y];
                        break;
                    case 0x1:
                        regs[X] |= regs[Y];
                        break; 
                    case 0x2:
                        regs[X] &= regs[Y];
                        break;
                    case 0x3:
                        regs[X] ^= regs[Y];
                        break;
                    case 0x4:
                        regs[0xF] = (regs[X] + regs[Y] > 0xFF);
                        regs[X] += regs[Y];
                        break;
                    case 0x5:
                        regs[0xF] = (regs[X] > regs[Y]);
                        regs[X] = regs[X] - regs[Y];
                        break;
                    case 0x6:
                        regs[0xF] = regs[X] & 0x1;
                        regs[X] >>= 1;
                        break;
                    case 0x7:
                        regs[15] = (regs[Y] > regs[X]);
                        regs[X] = regs[Y] - regs[X];
                        break;
                    case 0xE:
                        regs[0xF] = (regs[X] & (0x1 << 7)) >> 7;
                        regs[X] <<= 1;
                        break;
                }
                break;
            case 0x9:
                std::cout << "SKIP" << std::endl;
                if (regs[X] != regs[Y]) PC += 2;
                break;
            case 0xA:
                std::cout << "SET I To NNN" << std::endl;
                I = NNN;
                break;
            case 0xB:
                PC = NNN + regs[X];
                break;
            case 0xC:
                regs[X] = (rand() % 0xFF) & NN;
                break;
            case 0xD:
                {
                    std::cout << "DRAW" << std::endl;
                    byte startX = regs[X] % SCREEN_WIDTH;
                    byte startY = regs[Y] % SCREEN_HEIGHT;
                    regs[0xF] = 0;
                    for (byte i = 0; i < N; ++i) {
                        byte pixelWide = MEM[I + i];
                        byte y = startY + i;
                        if (y > SCREEN_HEIGHT) break;
                        for (byte k = 7; k >= 0; --k) {
                            byte bit = (pixelWide >> k) & 1;
                            byte x = startX + k;
                            if (x > SCREEN_WIDTH) break;
                            regs[0xF] = bit && display[x][y] ? 1 : 0;
                            display[x][y] ^= bit;
                        }
                    }
                }
                break;
            case 0xE:
                std::cout << "NONE" << std::endl;
                break;
            case 0xF:
                std::cout << "NONE" << std::endl;
                break;
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                handleKeyboard(e.key.keysym.scancode);
            }
        }

        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            for(int y = 0; y < SCREEN_HEIGHT; ++y) {
                if (display[x][y]) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                }
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);
        const int frameDelay = SPF  - (SDL_GetTicks() - startTime); 
        if (frameDelay > 0) {
            SDL_Delay(frameDelay);
        }

        if (sound_timer > 0) {
            // play sound
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    program.close();
}
