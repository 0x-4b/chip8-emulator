#include "chip8.hpp"

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
            memory[START_ADDRESS + 1] = buffer[i];
        }

        delete[] buffer;
    }
}