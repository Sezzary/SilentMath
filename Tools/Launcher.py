"""
Silent Engine Launcher

Runs a simple launcher which prompts ROM selection for asset extraction and starts the Silent Engine application.
"""

import customtkinter
import multiprocessing
import os
import platform
import subprocess
import sys

from pathlib       import Path
from customtkinter import filedialog

DUMPSXISO_NAME      = "dumpsxiso"
EXTRACT_ASSETS_NAME = "ExtractAssets.py"
BASE_PATH           = Path(__file__).parent
TEMP_BASE_PATH      = Path(getattr(sys, '_MEIPASS', os.path.abspath(".")))
TEMP_OUTPUT_PATH    = BASE_PATH / ".temp"
ASSETS_PATH         = BASE_PATH / "AssetsTest"
ASSETS_AUDIO_PATH   = ASSETS_PATH / "Audio"
ASSETS_VIDEO_PATH   = ASSETS_PATH / "Video"

def _get_dumpsxiso_exe():
    """
    Get the platform-specific `dumpsxiso` executable to use.
    """
    # Define executable path.
    system_os = platform.system().lower()
    if system_os == "windows":
        dumpsxiso_exe = os.path.join(TEMP_BASE_PATH, DUMPSXISO_NAME + ".exe")
    elif system_os == "darwin": # macOS
        dumpsxiso_exe = os.path.join(TEMP_BASE_PATH, DUMPSXISO_NAME)
    elif system_os == "linux":
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
    file_path = filedialog.askopenfilename(
        title="Select a Silent Hill ROM",
        filetypes=[("Silent Hill ROM Image", "*.bin")])

    if file_path:
        print(f"Selected: {file_path}")
        return file_path

    return None

def _dump_rom(romPath: str):
    # Setup.
    dumpsxiso_exe = _get_dumpsxiso_exe()

    # Run command.
    command = [dumpsxiso_exe,
               "-x", TEMP_OUTPUT_PATH,
               "-s", TEMP_OUTPUT_PATH / "Layout.xml",
               romPath]

    # Report status.
    result = subprocess.run(command)
    if result.returncode != 0:
        raise Exception(f"ROM dump failed: {result.stderr.decode()}")

def _extract_assets():
    # Setup.
    extract_assets_script = _get_extract_assets_script()

    # Run command.
    command = [extract_assets_script,
               ASSETS_PATH,
               "-exe", TEMP_OUTPUT_PATH / "SLUS_007.07",
               "-fs", TEMP_OUTPUT_PATH / "SILENT.",
               "-fh", TEMP_OUTPUT_PATH / "HILL."]

    # Report status.
    result = subprocess.run(command)
    if result.returncode != 0:
        raise Exception(f"Asset extraction failed: {result.stderr.decode()}")
            
def _convert_audio_and_video():
    # Run through `.XA` and `.STR` files.
    for file in (ASSETS_PATH / "XA").iterdir():
        # Run command.
        if file.suffix == ".XA":
            command = ["ffmpeg",
                       "-hide_banner", "-loglevel", "error", "-i", str(file),
                       ASSETS_AUDIO_PATH]
        elif file.suffix == ".STR":
            command = ["ffmpeg",
                       "-hide_banner", "-loglevel", "error",
                       "-i", str(file),
                       "-r", "30",
                       "-c:v", "mpeg1video", "-q:v", "1", "-bf", "0",
                       "-maxrate:v", "1500k", "-bufsize:v", "1835k",
                       "-vf", "format=yuv420p",
                       "-c:a", "mp2", "-ar", "44100", "-ac", "2",
                       "-f", "mpeg", ASSETS_VIDEO_PATH]
        else:
            continue

        # Report status.
        result = subprocess.run(command)
        if result.returncode != 0:
            raise Exception(f"Asset conversion failed for file `{file.name}`: {result.stderr.decode()}")

def main():
    multiprocessing.freeze_support()

    try:
        WIDTH  = 400
        HEIGHT = 500

        customtkinter.set_appearance_mode("System")

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
            _extract_assets()
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
