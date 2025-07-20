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
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
    }
}

void Chip8::OP_6xkk()
{
    // Set Vx = kk
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

void Chip8::OP_7xkk()
{
    // Set Vx = Vx + kk
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

void Chip8::OP_8xy0()
{
    // Set Vx = Vy
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
    // Set Vx = Vx OR Vy
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

    registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()
{
    // Set Vx = Vx AND Vy
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

    registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()
{
    // Set Vx = Vx XOR Vy
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
    // Set Vx = Vx + Vy, set VF = carry
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

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
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vy] -= registers[Vx];
}

void Chip8::OP_8xy6()
{
    // Set Vx = Vx >> 1, VF = least significant bit before shift
    u8 Vx = (opcode & 0x0F00) >> 8;
    registers[0xF] = (registers[Vx] & 0x1u);
    registers[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
    // Set Vx = Vy - Vx, set VF = NOT borrow
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

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
    u8 Vx = (opcode & 0x0F00u) >> 8;

    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
    // Skip next instruction if Vx != Vy
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;

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
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn()
{
    // Display n-byte sprite starting at memory[I] at (Vx, Vy), set VF = collision
    u8 Vx = (opcode & 0x0F00u) >> 8;
    u8 Vy = (opcode & 0x00F0u) >> 4;
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
    // Wait for a key press, store the value of the key in Vx
    u8 Vx = (opcode & 0x0F00) >> 8u;

    if (keypad[0])
    {
        registers[Vx] = 0;
    }

    else if (keypad[1])
    {
        registers[Vx] = 1;
    }

    else if (keypad[2])
    {
        registers[Vx] = 2;
    }

    else if (keypad[3])
    {
        registers[Vx] = 3;
    }

    else if (keypad[4])
    {
        registers[Vx] = 4;
    }

    else if (keypad[5])
    {
        registers[Vx] = 5;
    }

    else if (keypad[6])
    {
        registers[Vx] = 6;
    }

    else if (keypad[7])
    {
        registers[Vx] = 7;
    }

    else if (keypad[8])
    {
        registers[Vx] = 8;
    }

    else if (keypad[9])
    {
        registers[Vx] = 9;
    }

    else if (keypad[10])
    {
        registers[Vx] = 10;
    }

    else if (keypad[11])
    {
        registers[Vx] = 11;
    }

    else if (keypad[12])
    {
        registers[Vx] = 12;
    }

    else if (keypad[13])
    {
        registers[Vx] = 13;
    }

    else if (keypad[14])
    {
        registers[Vx] = 14;
    }

    else if (keypad[15])
    {
        registers[Vx] = 15;
    }

    else
    {
        pc += 2;
    }
}

void Chip8::OP_Fx15()
{
    // Set delay timer = Vx
    u8 Vx = (opcode & 0x0F00) > 8u;
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18()
{
    // Set sound timer = Vx
    u8 Vx = (opcode & 0x0F00) > 8u;
    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E()
{
    // Set I = I + Vx
    u8 Vx = (opcode & 0x0F00) > 8u;
    index += registers[Vx];
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
