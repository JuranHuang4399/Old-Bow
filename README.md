## How to Run

- Navigate to the `OldBow` folder.
- Run the executable file: **OldBow.exe**
- Make sure the following DLL files are in the same directory:
  - SDL2.dll
  - SDL2_ttf.dll

No compilation is required. The project includes the pre-compiled executable and all necessary dependencies.

## Notes

- All dialogues and scenes are data-driven and loaded from external txt files in the `Assets` folder.
	- Boss1.txt
	- Boss2.txt
	- Boss3.txt
- Doxygen-generated documentation is available in the `Doxygen` folder.

For details about the game design and future plans, please see:
- `Project.md` — Game engine description and video trailer
- `PostMortem.md` — Reflection and improvements

**Note:** This game was developed and tested on **Windows** using **Code::Blocks**, **MinGW 64-bit**, and **SDL2 64-bit**. 

If you're on Windows, you can run the provided executable directly.

If you're on macOS or Linux, or prefer to build the game yourself, please follow the instructions below.

## How to Compile

### Option 1: Build with Code::Blocks

1. Open the project file `OldBow.cbp` with Code::Blocks.
2. Ensure MinGW 64-bit compiler is installed and selected.
3. Make sure SDL2 and SDL2_ttf development libraries are properly linked.
4. Build the project using the "Build and Run" option.

### Option 2: Build with g++ manually

Open a terminal / bash, navigate to the `OldBow` directory, and run:

```bash
g++ -g -I"./include" -I"./lib/SDL2/include" ./src/*.cpp -o OldBow.exe -L"./lib/SDL2/lib" -lmingw32 -lSDL2 -lSDL2main -lSDL2_ttf
```