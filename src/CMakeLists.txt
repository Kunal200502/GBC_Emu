cmake_minimum_required(VERSION 3.5)
project(GBC_emu)

find_package(SDL2)
find_package(Boost COMPONENTS serialization)

set(
    SOURCE_FILES
    Bus/Bus.cpp
    Bus/IO_Registers.cpp
    Joypad/Joypad.cpp
    Cartridge/Cartridge.cpp
    Cpu/Cpu.cpp
    Cpu/Opcode_info.cpp
    Timer/Timer.cpp
    PPU/PPU.cpp
    PPU/LCDRenderer.cpp
    PPU/FIFOPixel.cpp
    PPU/OAMBuffer.cpp
    GameBoy/GameBoy.cpp
    GameBoy/SaveState.cpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::serialization)
