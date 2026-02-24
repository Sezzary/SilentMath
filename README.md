# <img src="https://github.com/Sezzary/SilentEngine/blob/master/Resources/Icon.png" alt="Icon" width="64" height="64"/> Silent Engine

*Silent Engine* is an in-progress cross-platform engine port designed to run the original *Silent Hill* on modern systems. It aims to be a monolithic, flexible, and future-proof foundation with modern standards that will enable long-term goals such as translation support and modding. Written from the ground-up with a high-quality, no-compromise codebase.

*Q: CAN I PLAY THIS?*
A: Not yet! There's still significant work to be done. Very little game logic resides in this repository so far, as the focus is on building a solid engine foundation before integrating gameplay and graphics. All considerations are being made to avoid technical debt. Additionally, the decompilation must be farther along before most of the porting work can begin. While basics like game menus are slowly underway, it's best to wait on the rest while the decompilation remains in constunt flux. Much of the original engine code is yet to be deobfuscated and documented.

Decompilation progress can be tracked here:
https://github.com/Vatuu/silent-hill-decomp

## Prospects

- Windows/macOS/Linux support out of the box, with the potential for other platforms later.
- Modern renderer supporting Vulkan, DX12, and Metal, with the potential for other backends in the future if needed.
- Various graphics toggles for a retro or modern look.
- Retro and HD font options. .TTF fonts matching the originals have been found or meticulously recreated.
- Clean, well-documented codebase.
- Lua scripting.
- Translation support.
- Modding support.
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
- Extensive warning and error logging
- Renderer with swappable backends
- Translator for internationalized scripts
- Font manager
- "Power" menu for deubgging
- Various utilities pulled from other projects

### TODOs

- Switch to GCC as the compiler. Works on Linux, Windows build has problems and relies on MSVC for now. Need MinGW?
- Forward renderer. Basic system abstraction is done, now it needs expansion.
- Sound system. Need to write a `KDC`+`VAB` -> `XM` converter?
- Parsers for all proprietary game file types.
- Lua scripting.

## Building (Windows/macOS/Linux)

NOTE: Project setup is yet to be streamlined and this section is incomplete. Ideally, the project will be able to cross-compile between all main development platforms.

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

Initialize Git LFS to ensure assets are pulled correctly:
```
git lfs install
```

Clone https://github.com/Sezzary/SilentEngine to your desired directory. Cloning recursively will include submodules in the process:
```
git clone --recursive https://github.com/Sezzary/SilentEngine
git submodule update --init --recursive
```

Build LuaJIT locally:
```
cd Libraries/LuaJIT && make && cd ../..
```

<details>
<summary>Windows</summary>
TODO: Steps incomplete.

Build shaders:
```
python Tools/GenerateShaders.py Windows
```
</details>

<details>
<summary>macOS</summary>
TODO: Steps incomplete.

Build shaders:
```
python Tools/GenerateShaders.py macOS
```
</details>

<details>
<summary>Linux</summary>

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

Build shaders:
```
python Tools/GenerateShaders.py Linux
```
</details>

### Transfer assets

Copy the `Assets` folder from the root of the repository to the `Build` folder. Then, extract all folders from the ROM's .SILENT archive to `Assets/Stream/Psx`.
Original game assets are not provided. Your own ROM of the 1.1 US release of the game must be sourced.

### Build the code (Debug/Release)

Available commands:
- Configure Debug:`cmake -S . -B Build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug`
- Build Debug: `cmake --build Build/Debug`

- Configure Release:`cmake -S . -B Build/Release -G Ninja -DCMAKE_BUILD_TYPE=Release`
- Build Release: `cmake --build Build/Release`

- Activate Python virtual environment: `source .venv/bin/activate`

## Contributing

Too early for this, but feel free to take a look around in the meantime!
