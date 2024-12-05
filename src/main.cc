#include <SDL/SDL.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fstream>
#include <random>
#include <ratio>
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



void logicDecode(byte X, byte Y, byte N, byte *regs) {
    switch(N) {
        case 0x0:
            regs[X] = regs[Y];
        case 0x1:
            regs[X] |= regs[Y];
        case 0x2:
            regs[X] &= regs[Y];
        case 0x3:
            regs[X] ^= regs[Y];
        case 0x4:
            regs[0xF] = regs[X] + regs[Y] < regs[X] ? 1 : 0;
            regs[X] += regs[Y];
        case 0x5:
            regs[0xF] = regs[X] > regs[Y];
            regs[X] -= regs[Y];
        case 0x6:
            regs[0xF] = regs[X] & 1;
            regs[X] >>= 1;
        case 0x7:
            regs[0xF] = regs[Y] > regs[X];
            regs[X] = regs[Y] - regs[X];
        case 0xE:
            regs[0xF] = regs[X] & (1 << 7);
            regs[X] <<= 1;
    }
}

char getKey(const SDL_Scancode &key) {
    switch (key) {
        case SDL_SCANCODE_0:
            std::cout << "key print 0" << std::endl;
            return 0;
        case SDL_SCANCODE_1:
            return 1;
        case SDL_SCANCODE_2:
            return 2;
        case SDL_SCANCODE_3:
            return 3;
        case SDL_SCANCODE_4:
            return 4;
        case SDL_SCANCODE_5:
            return 5;
        case SDL_SCANCODE_6:
            return 6;
        case SDL_SCANCODE_7:
            return 7;
        case SDL_SCANCODE_8:
            return 8;
        case SDL_SCANCODE_9:
            return 9;
        case SDL_SCANCODE_A:
            return 10;
        case SDL_SCANCODE_B:
            return 11;
        case SDL_SCANCODE_C:
            return 12;
        case SDL_SCANCODE_D:
            return 13;
        case SDL_SCANCODE_E:
            return 14;
        case SDL_SCANCODE_F:
            std::cout << "key print F" << std::endl;
            return 15;
        default:
            return 16;
    }
}

template<int FPS>
class Timer {
    using frameDuration = std::chrono::duration<double, std::ratio<1, FPS>>;
    std::chrono::time_point<std::chrono::steady_clock, frameDuration> tp;
public:
    Timer(): tp{std::chrono::steady_clock::now()} {}
    static constexpr frameDuration frameTick{1};

    void sleep() {
        tp += frameTick;
        while (std::chrono::steady_clock::now() < tp) {}
    }
};

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
    std::random_device dev;
    std::mt19937 rng{dev()};
    std::uniform_int_distribution<std::mt19937::result_type> distFF(0, 0xFF);

    Timer<60> frameCap; 
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

        bool isGetKey = false;
        bool isKeyCheck = false;

        switch(opcode & 0xF000) {
            case 0x0000:
                switch(NN) {
                    case 0xE0:
                        clear(renderer, display);
                        std::cout << "clear" << std::endl;
                    case 0xEE:
                        PC = return_addrs.top();
                        return_addrs.pop();
                        std::cout << "return" << std::endl;
                    default:
                        std::cout << "decode error" << std::endl;
                }
                break;
            case 0x1000:
                PC = NNN;
                std::cout << "jump" << std::endl;
                break;
            case 0x2000:
                return_addrs.push(PC);
                PC = NNN;
                std::cout << "call" << std::endl;
                break;
            case 0x3000:
                PC += regs[X] == NN ? 2 : 0;
                std::cout << "conditonal" << std::endl;
                break;
            case 0x4000:
                PC += regs[X] == NN ? 0 : 2;
                std::cout << "conditonal" << std::endl;
                break;
            case 0x5000:
                PC += regs[X] == regs[Y]  ? 2 : 0;
                std::cout << "conditonal" << std::endl;
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
                logicDecode(X, Y, N, regs);
                std::cout << "logical operation" << std::endl;
                break;
            case 0x9000:
                PC += regs[X] == regs[Y]  ? 0 : 2;
                std::cout << "conditonal" << std::endl;
                break;
            case 0xA000:
                I = NNN;
                std::cout << "set I" << std::endl;
                break;
            case 0xB000:
                PC = NNN + regs[X];
                std::cout << "jump offset" << std::endl;
                break;
            case 0xC000:
                regs[X] = distFF(rng) & NN;
                std::cout << "rand" << std::endl;
                break;
            case 0xD000:
                draw(regs, X,  Y, N, I, display);
                std::cout << "display" << std::endl;
                break;
            case 0xE000:
                {
                    SDL_PollEvent(&e);
                    bool isPressed = e.type == SDL_KEYDOWN && getKey(e.key.keysym.scancode) == regs[X];
                    if (NN == 0x9E) {
                        PC += isPressed ? 2 : 0;
                    } else if (NN == 0xA1) {
                        PC += isPressed ? 0 : 2; 
                    }
                    std::cout << "key check" << std::endl;
                }
                break;
            case 0xF000:
                switch (NN) {
                    case 0x07:
                        regs[X] = delay_timer;
                        break;
                    case 0x15:
                        delay_timer = regs[X];
                        break;
                    case 0x18:
                        sound_timer = regs[X];
                        break;
                    case 0x1E:
                        regs[0xF] = I + regs[X] > 0x1000 || I + regs[X] < I;
                        I += regs[X];
                        break;
                    case 0x0A:
                        isGetKey = true;
                        break;
                    case 0x29:
                        I =  80 + regs[X];
                        break;
                    case 0x33:
                        for (int i = 0; i < 3; ++i) {
                            MEM[I + i] = (regs[X] / (int) pow(10, 2 - i)) % 10;
                        }
                        break;
                    case 0x55:
                        break;
                    case 0x65:
                        break;
                }
                break;
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (isGetKey) {
                if (e.type == SDL_KEYDOWN) {
                    regs[X] = getKey(e.key.keysym.scancode);
                } else {
                    PC -= 2;
                }
                isGetKey = false;
            }
        }

        if (sound_timer > 0) {
            std::cout << '\a';
        }

        renderFrame(renderer, display);
        SDL_RenderPresent(renderer);
        delay_timer -= delay_timer > 0 ? 1 : 0;
        sound_timer -= sound_timer > 0 ? 1 : 0;
        frameCap.sleep();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = nullptr;
    renderer = nullptr;
    SDL_Quit();
}
