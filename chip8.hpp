#pragma once
#include <iostream>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <random>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

const unsigned int START_ADDRESS = 0x200;


class Chip8
{
    public:
        Chip8();
        u8 registers[16] {};
        u8 memory[4096] {};
        u16 index{};
        u16 pc{};
        u16 stack[16] {};
        u8 sp{};
        u8 delay_timer{};
        u8 sound_timer{};
        u8 keypad[16]{};
        u32 video[64 * 32] {};
        u16 opcode{};


        void LoadRoam(char* const filename);
};