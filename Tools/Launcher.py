"""
Silent Engine Launcher

Runs a simple launcher which prompts ROM selection for asset extraction and starts the Silent Engine application.
"""

import customtkinter
import multiprocessing
import os
import platform
import re
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

# Checksums for ROMs supported for asset extraction.
# @todo Support all final releases. Ideal setup would collect every translated puzzle texture from multiple ROMs.
SUPPORTED_RELEASE_CHECKSUMS = [
    #0x1532C55C, # NTSC-J   Rev 0 99-01-26       (SLPM-86192).
    0xCF9CD8E5, # NTSC 1.1 99-02-10             (SLUS-00707).
    #0xEB733CAA, # NTSC-J   Rev 1/Rev 2 99-06-02 (SLPM-86192).
    #0x337E4A60  # PAL      99-06-07             (SLES-01514).
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
    If FFmpeg is missing on the first call, it will be downloaded.
    """
    ffmpeg_exe, ffprobe_exe = run.get_or_fetch_platform_executables_else_raise()
    return ffmpeg_exe

def _get_dumpsxiso_exe():
    """
    Get the platform-specific `dumpsxiso` executable to use.
    """
    # Define executable path.
    system_os = platform.system().lower()
    if system_os == "windows":
        dumpsxiso_exe = os.path.join(TEMP_BASE_PATH, DUMPSXISO_NAME + ".exe")
    elif system_os == "darwin" or system_os == "linux": # `darwin` = macOS.
        dumpsxiso_exe = os.path.join(TEMP_BASE_PATH, DUMPSXISO_NAME)
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
    extract_assets_script = os.path.join(TEMP_BASE_PATH, EXTRACT_ASSETS_NAME)

    # Set permissions.
    system_os = platform.system().lower()
    if system_os in ["darwin", "linux"]:
        os.chmod(extract_assets_script, 0o755)

    return extract_assets_script

def _select_rom_file():
    """
    Show a file selection window to prompt a ROM selection.
    """
    file_path = filedialog.askopenfilename(
        title="Select a Silent Hill ROM",
        filetypes=[("Silent Hill ROM Image", "*.bin")])

    if file_path:
        print(f"Selected: {file_path}")
        return file_path

    return None

def _dump_rom(romPath: str):
    """
    Dump a supportedd Silent Hill ROM.
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
        romPath
    ]

    # Report status.
    result = subprocess.run(command)
    if result.returncode != 0:
        raise Exception(f"ROM dump failed: {result.stderr.decode()}")

    print("ROM dump completed successfully.")

def _check_supported_checksum():
    """
    Check if a given Silent Hill executable checksum matches that of a supported release.
    """
    # Setup.
    python_cmd            = _get_python_cmd()
    extract_assets_script = _get_extract_assets_script()

    # Run command.
    command = [
        python_cmd, extract_assets_script,
        ASSETS_PATH,
        "-exe", TEMP_BASE_PATH / "SLUS_007.07",
        "-fs", TEMP_BASE_PATH / "SILENT.",
        "-fh", TEMP_BASE_PATH / "HILL.",
        "-c"
    ]
    result = subprocess.run(command, capture_output=True, text=True)

    # Report status.
    if result.returncode != 0:
        raise Exception(f"Supported checksum check failed: {result.stderr}")

    # Confirm compatible checksum.
    full_output  = result.stdout + result.stderr
    checksum_str = re.search(r"Checksum of `.*?`: ([0-9A-F]{8})", full_output)
    if checksum_str:
        checksum = int(checksum_str.group(1), 16)
        return checksum in SUPPORTED_RELEASE_CHECKSUMS

    return False

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
            command = [
                ffmpeg_cmd,
                "-hide_banner", "-loglevel", "error", "-i", str(file),
                ASSETS_AUDIO_PATH / f"{file.stem}.WAV"
            ]

            print(f"Converting `{file.name}` to `{file.stem}.WAV`...")
        elif file.suffix == ".STR":
            command = [
                ffmpeg_cmd,
                "-hide_banner", "-loglevel", "error",
                "-i", str(file),
                "-r", "30",
                "-c:v", "mpeg1video", "-q:v", "1", "-bf", "0",
                "-maxrate:v", "1500k", "-bufsize:v", "1835k",
                "-vf", "format=yuv420p",
                "-c:a", "mp2", "-ar", "44100", "-ac", "2",
                "-f", "mpeg", ASSETS_VIDEO_PATH / f"{file.stem}.MPG"
            ]

            print(f"Converting `{file.name}` to `{file.stem}.MPG`...")
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
        WIDTH  = 400
        HEIGHT = 500

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
            romPath = _select_rom_file()
            if romPath:
                label.configure(text=f"Path: ...{romPath[-30:]}")

            _dump_rom(romPath)
            if not _check_supported_checksum():
                return

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
