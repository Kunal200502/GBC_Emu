# GBC_Emu
A GameBoy Color emulator in C++
Uses SDL2 library for rendering
Passes all blargg's cpu tests
Saving and loading functionality is very basic for now - only supports NoMBC games

## Usage
### To Build 
Create a build folder 
In the build folder -
```
cmake ../src/
cmake --build .
```

### To Exectute-
```
./GBC_emu path_to_rom_file
```

## Controls
| GameBoy buttons | Keyboard |
| :-------------: | :------: |
| right           | d        | 
| left            | a        | 
| up              | w        | 
| down            | s        | 
| A               | l        | 
| B               | j        | 
| Select          | i        | 
| Start           | k        | 
| Save            | F1       |
| Load            | F2       |