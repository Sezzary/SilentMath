# <img src="https://github.com/Sezzary/SilentEngine/blob/master/Resources/Icon.png" alt="Icon" width="64" height="64"/> Silent Engine

Track decompilation progress here: https://github.com/Vatuu/silent-hill-decomp

## Current foundations

- Math wrappers and functions (similar to DXTK but with some usability improvements)
- Input handling
- Savegame handling
- Config options handling
- Window handling
- Asset streaming
- Timestep handling
- Parallel task handling
- Filesystem handling
- Clean, extensive logging
- Renderer with swappable backends
- Sound system
- Translator for internationalized scripts
- Font manager
- "Power" menu for deubgging
- Various utilities pulled from other projects

All considerations are being made to establish a flexible, readable, and accessible foundation. Portability is in mind as a top priority for cross-platform support from inception.

### TODOs

- The decomp must be farther along before the bulk of the porting work can begin. More basic things like the boot screen and game menus could be ported already, but it's best to wait for the rest while the decomp is in constunt flux and much of the engine code remains to be deobfuscated.
- Switch to GCC as the compiler. Works on Linux, Windows build has problems and relies on MSVC for now.
- Forward renderer. Basic system abstraction is done, now it needs expansion.
- Sound system. Need to write a `KDC`+`VAB` -> `XM` converter?
- Parsers for all proprietary game file types.
- Test math classes.
- Extensive documentation.
- Lua scripting.

## Building (Windows/macOS/Linux)

WIP! Ideally, the project will be able to cross-compile between all main development platforms.

### Install dependencies

The project has the following requirements:
- cmake
- git
- ninja
- pip
- python3

<details>
<summary>Windows</summary>

```
winget install Kitware.CMake Git.Git Ninja-build.Ninja Python.Python.3
```
</details> <details> <summary>macOS</summary>

```
# TODO: Add macOS instructions here
```
</details> <details> <summary>Linux</summary>

```
sudo apt install build-essential git ninja-build python3 python3-pip
```
</details>

### Clone the repository

Clone https://github.com/Sezzary/SilentEngine to your desired directory. Cloning recursively will ensure the following submodules are included in the process:
- assimp
- FreeType
- GLM
- HarfBuzz
- ImGui
- ImGuizmo
- json
- LuaJIT
- SDL3
- smol-atlas
- sol2
- spdlog + {fmt}
- stb
- UTF8-CPP
- VLC
- yalantinglibs
```
git clone --recursive https://github.com/Sezzary/SilentEngine
git submodule update --init --recursive
```

Build LuaJIT locally:
```
cd Libraries/LuaJIT && make && cd ../..
```

<details>
<summary>Linux</summary>

```
sudo apt install build-essential git ninja-build python3 python3-pip
```

Install SDL dependencies:
```
sudo apt-get install build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev
```

Set up Python virtual environment:
```
python3 -m venv .venv
source .venv/bin/activate
```
</details>

### Build the code (Debug/Release)

Available commands:
- Configure Debug:`cmake -S . -B Build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug`
- Build Debug: `cmake --build Build/Debug`

- Configure Release:`cmake -S . -B Build/Release -G Ninja -DCMAKE_BUILD_TYPE=Release`
- Build Release: `cmake --build Build/Release`

- Activate Python virtual environment: `source .venv/bin/activate`

## Contributing

Too early for this, but feel free to take a look around in the meantime!
