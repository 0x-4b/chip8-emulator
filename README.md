# CHIP-8 Emulator

[![C++](https://img.shields.io/badge/C%2B%2B-17%2B-blue.svg)](https://en.cppreference.com/)
[![SDL2](https://img.shields.io/badge/SDL2-2.0%2B-green.svg)](https://www.libsdl.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A fully functional CHIP-8 emulator written in C++ with SDL2 for graphics and input. Experience classic 8-bit games like Pong, Tetris, and Space Invaders with accurate emulation and modern performance.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Controls](#controls)
- [Project Structure](#project-structure)
- [Technical Details](#technical-details)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Author](#author)

---

## Overview

**CHIP-8 Emulator** is a cycle-accurate interpreter for the CHIP-8 virtual machine, originally designed in the 1970s for early microcomputers. This emulator faithfully recreates the CHIP-8 system, allowing you to run classic games and programs with modern graphics scaling and responsive controls.

---

## Features

- 🎮 **Full CHIP-8 instruction set** - All 35 opcodes implemented
- 🖥️ **SDL2 graphics** - Smooth rendering with configurable scaling
- ⌨️ **Hexadecimal keypad** - Standard CHIP-8 input mapping
- 🎵 **Sound support** - Beep functionality with sound timer
- ⚡ **Configurable timing** - Adjustable CPU cycle delay
- 🔧 **Easy ROM loading** - Simple command-line ROM loading
- 📊 **Debug output** - Real-time pixel counting for debugging

---

## Installation

### Prerequisites
- **C++17** compatible compiler (GCC, Clang, or MSVC)
- **SDL2** development libraries
- **pkg-config** (for automatic SDL2 linking)

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential libsdl2-dev pkg-config
```

### macOS
```bash
brew install sdl2 pkg-config
```

### Build Instructions
1. **Clone the repository:**
   ```bash
   git clone https://github.com/0x-4b/chip8-emulator.git
   cd chip8-emulator
   ```

2. **Compile the emulator:**
   ```bash
   make
   ```

---

## Usage

Run the emulator with the following command:

```bash
./chip8 <Scale> <Delay> <ROM>
```

**Parameters:**
- `<Scale>`: Window scaling factor (e.g., 10 for 640x320 window)
- `<Delay>`: CPU cycle delay in milliseconds (e.g., 3)
- `<ROM>`: Path to the CHIP-8 ROM file

**Example:**
```bash
./chip8 10 3 roms/PONG
```

---

## Controls

The CHIP-8 uses a hexadecimal keypad (0-F). The mapping is:

```
CHIP-8 Keypad    Your Keyboard
┌─┬─┬─┬─┐        ┌─┬─┬─┬─┐
│1│2│3│C│        │1│2│3│4│
├─┼─┼─┼─┤        ├─┼─┼─┼─┤
│4│5│6│D│   →    │Q│W│E│R│
├─┼─┼─┼─┤        ├─┼─┼─┼─┤
│7│8│9│E│        │A│S│D│F│
├─┼─┼─┼─┤        ├─┼─┼─┼─┤
│A│0│B│F│        │Z│X│C│V│
└─┴─┴─┴─┘        └─┴─┴─┴─┘
```

**Additional Controls:**
- `ESC` - Exit emulator

---

## Project Structure

```
chip8-emulator/
├── src/
│   ├── Chip8.cpp          # Core emulator logic and opcodes
│   ├── Chip8.hpp          # Emulator class definition
│   ├── Main.cpp           # Entry point and main loop
│   ├── Platform.cpp       # SDL2 wrapper for graphics/input
│   └── Platform.hpp       # Platform class definition
├── Makefile               # Build configuration
├── .gitignore            # Git ignore rules
├── README.md             # This file
└── LICENSE               # MIT License
```

---

## Technical Details

### CHIP-8 System Specifications
- **Memory**: 4KB RAM (0x000-0xFFF)
- **Display**: 64x32 monochrome pixels
- **Registers**: 16 8-bit general-purpose registers (V0-VF)
- **Stack**: 16 levels for subroutine calls
- **Timers**: 60Hz delay and sound timers
- **Input**: 16-key hexadecimal keypad

### Emulator Architecture
- **Instruction decoding** via function pointer tables for performance
- **Cycle-accurate timing** with configurable delay
- **Memory-mapped fontset** at 0x50-0x9F
- **Program loading** at standard address 0x200
- **XOR-based graphics** rendering for authentic flicker effects

### Supported Opcodes
All 35 standard CHIP-8 instructions including:
- Arithmetic and logic operations
- Memory access and manipulation
- Control flow (jumps, calls, returns)
- Graphics rendering with collision detection
- Input handling and timers

---

## Roadmap

- [ ] Add support for SCHIP (Super-CHIP) extensions
- [ ] Implement save states functionality
- [ ] Add ROM compatibility database
- [ ] Create GUI for ROM selection and settings
- [ ] Add sound wave customization
- [ ] Implement debugger with step-through capability

---

## Contributing

Pull requests and suggestions are welcome!  
To contribute:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/your-feature`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature/your-feature`)
5. Open a Pull Request

Please ensure your code follows the existing style and includes appropriate comments.

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Author

**0x-4b**  
GitHub: [@0x-4b](https://github.com/0x-4b)

---
