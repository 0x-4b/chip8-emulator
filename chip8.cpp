#include "chip8.hpp"
#include <cstdint>
#include <cstring>
#include <chrono>
#include <fstream>

uint8_t fontset[FONTSET_SIZE] =
    {
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

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = START_ADDRESS;

    randByte = std::uniform_int_distribution<u8>(0, 255U);

    for (long i = 0; i < FONTSET_SIZE; i++)
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

void Chip8::LoadRoam(char *const filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        std::streampos size = file.tellg();
        char *buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; i++)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
}

void Chip8::OP_00E0()
{
    // Clear the display
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
    // Return from subroutine
    --sp;
    pc = stack[sp];
}

void Chip8::OP_0nnn()
{
    // SYS addr (ignored in modern interpreters)
}

void Chip8::OP_1nnn()
{
    // Jump to location nnn
    u16 address = opcode & 0x0FFFu;
    pc = address;
}

void Chip8::OP_2nnn()
{
    // Call subroutine at nnn
    u16 address = opcode & 0x0FFFu;
    stack[sp] = pc;
    ++sp;
    pc = address;
}

void Chip8::OP_3xkk()
{
    // Skip next instruction if Vx == kk
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 byte = opcode & 0x00FFu;

    if (registers[Vx] == byte)
    {
        pc += 2;
    }
}

void Chip8::OP_4xkk()
{
    // Skip next instruction if Vx != kk
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 byte = opcode & 0x00FFu;

    if (registers[Vx] != byte)
    {
        pc += 2;
    }
}

void Chip8::OP_5xy0()
{
    // Skip next instruction if Vx == Vy
}

void Chip8::OP_6xkk()
{
    // Set Vx = kk
}

void Chip8::OP_7xkk()
{
    // Set Vx = Vx + kk
}

void Chip8::OP_8xy0()
{
    // Set Vx = Vy
}

void Chip8::OP_8xy1()
{
    // Set Vx = Vx OR Vy
}

void Chip8::OP_8xy2()
{
    // Set Vx = Vx AND Vy
}

void Chip8::OP_8xy3()
{
    // Set Vx = Vx XOR Vy
}

void Chip8::OP_8xy4()
{
    // Set Vx = Vx + Vy, set VF = carry
}

void Chip8::OP_8xy5()
{
    // Set Vx = Vx - Vy, set VF = NOT borrow
}

void Chip8::OP_8xy6()
{
    // Set Vx = Vx >> 1, VF = least significant bit before shift
}

void Chip8::OP_8xy7()
{
    // Set Vx = Vy - Vx, set VF = NOT borrow
}

void Chip8::OP_8xyE()
{
    // Set Vx = Vx << 1, VF = most significant bit before shift
}

void Chip8::OP_9xy0()
{
    // Skip next instruction if Vx != Vy
}

void Chip8::OP_Annn()
{
    // Set I = nnn
}

void Chip8::OP_Bnnn()
{
    // Jump to location nnn + V0
}

void Chip8::OP_Cxkk()
{
    // Set Vx = random byte AND kk
}

void Chip8::OP_Dxyn()
{
    // Display n-byte sprite starting at memory[I] at (Vx, Vy), set VF = collision
}

void Chip8::OP_Ex9E()
{
    // Skip next instruction if key with the value of Vx is pressed
}

void Chip8::OP_ExA1()
{
    // Skip next instruction if key with the value of Vx is not pressed
}

void Chip8::OP_Fx07()
{
    // Set Vx = delay timer value
}

void Chip8::OP_Fx0A()
{
    // Wait for a key press, store the value of the key in Vx
}

void Chip8::OP_Fx15()
{
    // Set delay timer = Vx
}

void Chip8::OP_Fx18()
{
    // Set sound timer = Vx
}

void Chip8::OP_Fx1E()
{
    // Set I = I + Vx
}

void Chip8::OP_Fx29()
{
    // Set I = location of sprite for digit Vx
}

void Chip8::OP_Fx33()
{
    // Store BCD representation of Vx in memory locations I, I+1, and I+2
}

void Chip8::OP_Fx55()
{
    // Store registers V0 through Vx in memory starting at location I
}

void Chip8::OP_Fx65()
{
    // Read registers V0 through Vx from memory starting at location I
}
