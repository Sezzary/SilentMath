# <img src="https://github.com/Sezzary/SilentEngine/blob/master/Resources/Icon.png" alt="Icon" width="64" height="64"/> Silent Engine

*Silent Engine* is an in-progress cross-platform engine port designed to run the original *Silent Hill* on modern systems. It aims to be a monolithic, flexible, future-proof foundation with modern features, while enabling long-term goals such as modding and translation support. Written from the ground-up with a high-quality, no-compromise codebase.

*Q: CAN I PLAY THIS?*
A: Not yet! There's still significant work to be done. Very little game logic resides in this repository so far, as the focus is on building a solid engine foundation before integrating gameplay and graphics. All considerations are being made to avoid technical debt and the decompilation must be farther along before most of the porting work can begin. While basics like game menus are slowly underway, it's best to wait on the rest while the decompilation remains in constunt flux and much of the engine code is yet to be deobfuscated.

Decompilation progress can be tracked here:
https://github.com/Vatuu/silent-hill-decomp

## Prospects

- Windows/macOS/Linux support out of the box, with potential support for other platforms later.
- Modern *SDL_gpu* renderer supporting Vulkan, DX12, and Metal. The backend is swappable and others can be written in the future if needed.
- Retro and HD font options. .TTF fonts matching the originas have been found or meticulously recreated.
- Easy internationalization.
- Easy modding.
- Clean, well-documented codebase.
- Lua scripting.
- Many others.

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

### TODOs

- Switch to GCC as the compiler. Works on Linux, Windows build has problems and relies on MSVC for now. Need MinGW?
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
