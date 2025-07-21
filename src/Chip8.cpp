#include "Chip8.hpp"
#include <cstdint>
#include <cstring>
#include <chrono>
#include <fstream>
#include <iostream>

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

    for (long i = 0; i < FONTSET_SIZE; i++)
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    randByte = std::uniform_int_distribution<u8>(0, 255U);

    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &Chip8::OP_NULL;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

void Chip8::Table0()
{
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
}
void Chip8::LoadROM(const char *filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cerr << "Failed to open ROM: " << filename << '\n';
        return;
    }

    std::streampos size = file.tellg();
    std::vector<char> buffer(size);

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), size);
    file.close();

    for (long i = 0; i < size; ++i)
    {
        memory[0x200 + i] = buffer[i];
    }

    std::cout << "ROM loaded into memory at 0x200, size: " << size << " bytes\n";
}
void Chip8::Cycle()
{
    opcode = (memory[pc] << 8) | memory[pc + 1]; // Make opcode a class member or pass it properly

    ((*this).*(table[(opcode & 0xF000) >> 12u]))();

    if (!((opcode & 0xF000) == 0x1000 || // 1nnn - jump
          (opcode & 0xF000) == 0x2000 || // 2nnn - call
          opcode == 0x00EE ||            // 00EE - return
          (opcode & 0xF000) == 0xB000 || // Bnnn - jump + V0
          // Add the conditional skip instructions:
          ((opcode & 0xF000) == 0x3000) || // 3xkk
          ((opcode & 0xF000) == 0x4000) || // 4xkk
          ((opcode & 0xF000) == 0x5000) || // 5xy0
          ((opcode & 0xF000) == 0x9000) || // 9xy0
          ((opcode & 0xF0FF) == 0xE09E) || // Ex9E
          ((opcode & 0xF0FF) == 0xE0A1)))  // ExA1
    {
        pc += 2;
    }

    if (delayTimer > 0)
        delayTimer--;
    if (soundTimer > 0)
        soundTimer--;
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
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 byte = opcode & 0x00FFu;

    if (registers[Vx] == byte)
    {
        pc += 2;
    }
}

void Chip8::OP_4xkk()
{
    // Skip next instruction if Vx != kk
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 byte = opcode & 0x00FFu;

    if (registers[Vx] != byte)
    {
        pc += 2;
    }
}

void Chip8::OP_5xy0()
{
    // Skip next instruction if Vx == Vy
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
    }
}

void Chip8::OP_6xkk()
{
    // Set Vx = kk
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

void Chip8::OP_7xkk()
{
    // Set Vx = Vx + kk
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

void Chip8::OP_8xy0()
{
    // Set Vx = Vy
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
    // Set Vx = Vx OR Vy
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()
{
    // Set Vx = Vx AND Vy
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()
{
    // Set Vx = Vx XOR Vy
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
    // Set Vx = Vx + Vy, set VF = carry
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    u8 sum = registers[Vx] + registers[Vy];

    if (sum > 255u)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
    // Set Vx = Vx - Vy, set VF = NOT borrow
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()
{
    // Set Vx = Vx >> 1, VF = least significant bit before shift
    u8 Vx = (opcode & 0x0F00) >> 8u;
    registers[0xF] = (registers[Vx] & 0x1u);
    registers[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
    // Set Vx = Vy - Vx, set VF = NOT borrow
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE()
{
    // Set Vx = Vx << 1, VF = most significant bit before shift
    u8 Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
    // Skip next instruction if Vx != Vy
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy])
    {
        pc += 2;
    }
}

void Chip8::OP_Annn()
{
    // Set I = nnn
    u16 address = opcode & 0x0FFFu;

    index = address;
}

void Chip8::OP_Bnnn()
{
    // Jump to location nnn + V0
    u16 address = opcode & 0x0FFFu;

    pc = registers[0] + address;
}

void Chip8::OP_Cxkk()
{
    // Set Vx = random byte AND kk
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn()
{
    // Display n-byte sprite starting at memory[I] at (Vx, Vy), set VF = collision
    u8 Vx = (opcode & 0x0F00u) >> 8u;
    u8 Vy = (opcode & 0x00F0u) >> 4u;
    u8 height = opcode & 0x000Fu;

    u8 xPos = registers[Vx] % VIDEO_WIDTH;
    u8 yPos = registers[Vy] % VIDEO_HEIGHT;
    registers[0xF] = 0;
    for (unsigned int row = 0; row < height; ++row)
    {
        u8 spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            u8 spritePixel = spriteByte & (0x80u >> col);
            u32 *screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
            if (spritePixel)
            {
                if (*screenPixel == 0xFFFFFFFF)
                {
                    registers[0xF] = 1;
                }

                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OP_Ex9E()
{
    // Skip next instruction if key with the value of Vx is pressed
    u8 Vx = (opcode & 0x0F00) >> 8u;
    u8 key = registers[Vx];

    if (keypad[key])
    {
        pc += 2;
    }
}

void Chip8::OP_ExA1()
{
    // Skip next instruction if key with the value of Vx is not pressed
    u8 Vx = (opcode & 0x0F00) >> 8u;
    u8 key = registers[Vx];

    if (!keypad[key])
    {
        pc += 2;
    }
}

void Chip8::OP_Fx07()
{
    // Set Vx = delay timer value
    u8 Vx = (opcode & 0x0F00) >> 8u;
    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A()
{
    u8 Vx = (opcode & 0x0F00) >> 8u;

    for (u8 key = 0; key < 16; ++key)
    {
        if (keypad[key])
        {
            registers[Vx] = key;
            pc += 2; // Advance only when a key is pressed
            return;
        }
    }
    // If no key pressed, do NOT advance pc — wait here
}

void Chip8::OP_Fx15()
{
    // Set delay timer = Vx
    u8 Vx = (opcode & 0x0F00) >> 8u;
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18()
{
    // Set sound timer = Vx
    u8 Vx = (opcode & 0x0F00) >> 8u;
    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E()
{
    // Set I = I + Vx
    u8 Vx = (opcode & 0x0F00) >> 8u;
    index += registers[Vx];
}

void Chip8::OP_Fx29()
{
    // Set I = location of sprite for digit Vx
    u8 Vx = (opcode & 0x0F00) >> 8u;
    u8 digit = registers[Vx];

    index = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx33()
{
    // Store BCD representation of Vx in memory locations I, I+1, and I+2
    u8 Vx = (opcode & 0x0F00) >> 8u;
    u8 value = registers[Vx];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
}

void Chip8::OP_Fx55()
{
    // Store registers V0 through Vx in memory starting at location I
    u8 Vx = (opcode & 0x0F00) >> 8u;

    for (u8 i = 0; i <= Vx; ++i)
    {
        memory[index + i] = registers[i];
    }
}

void Chip8::OP_Fx65()
{
    // Read registers V0 through Vx from memory starting at location I
    u8 Vx = (opcode & 0x0F00) >> 8u;

    for (u8 i = 0; i <= Vx; ++i)
    {
        registers[i] = memory[index + i];
    }
}

void Chip8::OP_NULL() {}