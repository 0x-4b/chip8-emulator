#include "Chip8.hpp"
#include "Platform.hpp"
#include <chrono>
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char *const romFilename = argv[3];

	Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

	Chip8 chip8;
	chip8.LoadROM(romFilename);

	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform.ProcessInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.Cycle();
			int activePixels = 0;
			for (int i = 0; i < VIDEO_WIDTH * VIDEO_HEIGHT; ++i)
			{
				if (chip8.video[i])
					++activePixels;
			}
			std::cout << "Pixels on: " << activePixels << std::endl;
			platform.Update(chip8.video, videoPitch);
		}
	}

	return 0;
}
