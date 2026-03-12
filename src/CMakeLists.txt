cmake_minimum_required(VERSION 3.5)
project(GBC_emu)

find_package(SDL2)

set(
    SOURCE_FILES
    Bus/Bus.cpp
    Cartridge/Cartridge.cpp
    Cpu/Cpu.cpp
    Cpu/Opcode_info.cpp
    Timer/Timer.cpp
    PPU/PPU.cpp
    GameBoy/GameBoy.cpp
    GameBoy/LCDRenderer.cpp
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
