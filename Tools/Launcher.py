"""
Silent Engine Launcher

Runs a simple launcher which prompts ROM selection for asset extraction and starts the Silent Engine application.
"""

import customtkinter
import hashlib
import multiprocessing
import os
import platform
import shutil
import subprocess
import sys

from customtkinter import filedialog
from pathlib       import Path
from static_ffmpeg import run

DUMPSXISO_NAME      = "dumpsxiso"
EXTRACT_ASSETS_NAME = "ExtractAssets.py"
BASE_PATH           = Path(sys.executable).parent
TEMP_BASE_PATH      = Path(getattr(sys, '_MEIPASS', os.path.abspath(".")))
ASSETS_PATH         = BASE_PATH / "Assets/Stream/Psx"
ASSETS_AUDIO_PATH   = BASE_PATH / "Assets/Audio"
ASSETS_VIDEO_PATH   = BASE_PATH / "Assets/Video"

# Checksums for supported ROMs.
ROM_CHECKSUMS = [
    0x80145C264A974B210E75A020967F10B7F806E22C, # NTSC-J   Rev 0 99-01-26       (SLPM-86192).
    0x34278D31D9B9B12B3B5DB5E45BCBE548991ECBC7, # NTSC 1.1 99-02-10             (SLUS-00707).
    0x83B8D89398829493457A48BE9820300ADE8703D4, # NTSC-J   Rev 1/Rev 2 99-06-02 (SLPM-86192).
    0xCA67A93E507B999A0040242362162E442F2BA07E  # PAL      99-06-07             (SLES-01514).
]

def _get_python_cmd():
    """
    Get the platform-specific system Python command.
    """
    system_os = platform.system().lower()
    return "python" if system_os == "windows" else "python3"

def get_ffmpeg_cmd():
    """
    Get the platform-specific system FFmpeg command.
    If FFmpeg is missing, it will fall back on a static download.
    """
    ffmpeg_exe = shutil.which("ffmpeg")
    if ffmpeg_exe:
        return ffmpeg_exe

    ffmpeg_exe = run.get_or_fetch_platform_executables_else_raise()
    return ffmpeg_exe

def _get_dumpsxiso_exe():
    """
    Get the platform-specific `dumpsxiso` executable to use.
    """
    # Define executable path.
    system_os = platform.system().lower()
    if system_os == "windows":
        dumpsxiso_exe = TEMP_BASE_PATH / f"{DUMPSXISO_NAME}.exe"
    elif system_os == "darwin" or system_os == "linux": # `darwin` = macOS.
        dumpsxiso_exe = TEMP_BASE_PATH / DUMPSXISO_NAME
    else:
        raise Exception(f"'{system_os}' is unsupported.")

    if not os.path.isfile(dumpsxiso_exe):
        raise Exception(f"`{DUMPSXISO_NAME}` executable not found at '{dumpsxiso_exe}'.")

    # Set permissions.
    if system_os in ["darwin", "linux"]:
        os.chmod(dumpsxiso_exe, 0o755)

    return dumpsxiso_exe

def _get_extract_assets_script():
    """
    Get the `ExtractAssets.py` script to use.
    """
    # Define script path.
    extract_assets_script = TEMP_BASE_PATH / EXTRACT_ASSETS_NAME

    # Set permissions.
    system_os = platform.system().lower()
    if system_os in ["darwin", "linux"]:
        os.chmod(extract_assets_script, 0o755)

    return extract_assets_script

def _generate_sha1(file_path: Path):
    """
    Generate a SHA-1 checksum from a file.
    """
    sha1_hash = hashlib.sha1()
    with open(file_path, "rb") as _file:
        for byte_block in iter(lambda: _file.read(4096), b""):
            sha1_hash.update(byte_block)

    return int(sha1_hash.hexdigest(), 16)

def _select_rom_file():
    """
    Show a file selection window to prompt ROM selection.
    """
    file_path = filedialog.askopenfilename(
        title="Select a Silent Hill ROM",
        filetypes=[("Silent Hill ROM Image", "*.bin")])
    
    checksum = _generate_sha1(file_path)
    if checksum not in ROM_CHECKSUMS:
        print(f"Unsupported ROM with checksum {checksum}.")
        return None

    if file_path:
        print(f"Selected: {file_path}")
        return file_path

    return None

def _dump_rom(rom_path: str):
    """
    Dump a supported Silent Hill ROM.
    Exported to `TEMP_BASE_PATH`.
    """
    print("Dumping ROM...")

    # Setup.
    dumpsxiso_exe = _get_dumpsxiso_exe()

    # Run command.
    command = [
        dumpsxiso_exe,
        "-x", TEMP_BASE_PATH,
        "-s", TEMP_BASE_PATH / "Layout.xml",
        rom_path
    ]

    # Report status.
    result = subprocess.run(command)
    if result.returncode != 0:
        raise Exception(f"ROM dump failed: {result.stderr.decode()}")

    print("ROM dump completed successfully.")

def _extract_assets(rom_exe: str):
    """
    Extract assets from dumped Silent Hill ROM data.
    Exported to `TEMP_BASE_PATH`.
    """
    # Setup.
    python_cmd            = _get_python_cmd()
    extract_assets_script = _get_extract_assets_script()

    # Run command.
    command = [
        python_cmd, extract_assets_script,
        ASSETS_PATH,
        "-exe", TEMP_BASE_PATH / rom_exe,
        "-fs", TEMP_BASE_PATH / "SILENT.",
        "-fh", TEMP_BASE_PATH / "HILL."
    ]
    result = subprocess.run(command)

    # Report status.
    if result.returncode != 0:
        raise Exception(f"Asset extraction failed: {result.stderr.decode()}")
            
def _convert_audio_and_video():
    """
    Convert `.XA` (audio) and `.STR` (video) asset files to usable formats.
    Exported to `ASSETS_AUDIO_PATH` and `ASSETS_VIDEO_PATH`.
    """
    print("Converting audio and video...")

    # Setup.
    ffmpeg_cmd = get_ffmpeg_cmd()

    # Create folders.
    ASSETS_AUDIO_PATH.mkdir(parents=True, exist_ok=True)
    ASSETS_VIDEO_PATH.mkdir(parents=True, exist_ok=True)

    # Run through `.XA` and `.STR` files.
    for file in (ASSETS_PATH / "XA").iterdir():
        # Run command.
        if file.suffix == ".XA":
            newFile = f"{file.stem}.WAV"
            command = [
                ffmpeg_cmd, "-y",
                "-hide_banner",
                "-loglevel", "error",
                "-i", str(file),
                ASSETS_AUDIO_PATH / newFile
            ]

            print(f"Converting `{file.name}` to `{newFile}`...")
        elif file.suffix == ".STR":
            newFile = f"{file.stem}.MPG"
            command = [
                ffmpeg_cmd, "-y",
                "-hide_banner",
                "-loglevel", "error",
                "-i", str(file),
                "-r", "30",
                "-c:v", "mpeg1video",
                "-q:v", "1",
                "-bf", "0",
                "-maxrate:v", "1500k",
                "-bufsize:v", "1835k",
                "-vf", "format=yuv420p",
                "-c:a", "mp2",
                "-ar", "44100",
                "-ac", "2",
                "-f", "mpeg",
                ASSETS_VIDEO_PATH / newFile
            ]

            print(f"Converting `{file.name}` to `{newFile}`...")
        else:
            continue
        result = subprocess.run(command)

        # Report status.
        if result.returncode != 0:
            raise Exception(f"Asset conversion failed for file `{file.name}`: {result.stderr.decode()}")

    print("Audio and video conversion complete.")

def main():
    multiprocessing.freeze_support()

    try:
        WIDTH  = 500
        HEIGHT = 400

        customtkinter.set_appearance_mode("Dark")

        # Create window object.
        root = customtkinter.CTk()
        root.title("Silent Engine Launcher")
        root.resizable(False, False)
        root.geometry(f"{WIDTH}x{HEIGHT}")

        # Add label.
        label = customtkinter.CTkLabel(root, text="Select a Silent Hill ROM.")
        label.pack(expand=True)

        def handle_click():
            # Get ROM path
            rom_path = _select_rom_file()
            if rom_path:
                label.configure(text=f"Path: ...{rom_path[-30:]}")

                _dump_rom(rom_path)
                _extract_assets("SLUS_007.07") # @todo Dehardcode executable.
                _convert_audio_and_video()

        button = customtkinter.CTkButton(root, text="Browse Files", command=handle_click)
        button.pack(expand=True)

        # Run window.
        root.mainloop()
    except Exception as ex:
        # Report exception.
        print(f"Error: {ex}")
        sys.exit(1)

if __name__ == "__main__":
    main()
