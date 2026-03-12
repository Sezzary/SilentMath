"""
Silent Engine Launcher Generator

Generates a launcher application for the active platform.
If a generated launcher already exist and is outdated, it will be overwritten.
"""

"""
@todo Need to ask the user to run these commands.
sudo apt install python3-tk
pip install customtkinter
"""

import os
import platform
import shutil
import subprocess
import sys

from pathlib import Path

LAUNCHER_NAME    = "Launcher.py"
SPEC_NAME        = "Launcher.spec"
BASE_PATH        = Path(__file__).parent
OUTPUT_PATH      = BASE_PATH / "../Build/Launcher"
TEMP_OUTPUT_PATH = OUTPUT_PATH / ".temp"
LAUNCHER_SCRIPT  = BASE_PATH / LAUNCHER_NAME
SPEC_FILE        = OUTPUT_PATH / SPEC_NAME

def _get_platform_name():
    """
    Get the name of the active platform.
    """
    system_os = platform.system().lower()
    if system_os == "windows":
        platform_name = "Windows"
    elif system_os == "darwin": # macOS
        platform_name = "macOs"
    elif system_os == "linux":
        platform_name = "Linux"
    else:
        raise Exception(f"'{system_os}' is unsupported.")

    return platform_name

def _cleanup():
    """
    Delete temporary build files.
    """
    shutil.rmtree(TEMP_OUTPUT_PATH, ignore_errors=True)
    SPEC_FILE.unlink(missing_ok=True)

def main():
    try:
        print("Generating launcher...")
        _cleanup()

        # Setup.
        platform_name = _get_platform_name()
        exe_ext       = ".exe" if platform_name == "Windows" else ""
        colon         = ";"    if platform_name == "Windows" else ":"
        launcher_exe  = OUTPUT_PATH / f"Launcher{exe_ext}"

        # Check if new launcher build is required.
        if launcher_exe.exists():
            run_new_build = os.path.getmtime(LAUNCHER_SCRIPT) > os.path.getmtime(launcher_exe)
        else:
            run_new_build = True

        # Run generation command.
        if run_new_build:
            command = ["pyinstaller", "--onefile", "--windowed", "--noconfirm",
                       "--add-data", f"../../Tools/ExtractAssets.py{colon}.",
                       "--add-binary", f"../../Tools/dumpsxiso/{platform_name}/dumpsxiso{exe_ext}{colon}.",
                       "--distpath", "Build/Launcher",
                       "--workpath", "Build/Launcher/.temp",
                       "--specpath", "Build/Launcher",
                       "Tools/Launcher.py"]
            result  = subprocess.run(command)

            # Report status.
            if result.returncode == 0:
                print("Launcher generated successfully.")
            else:
                print(f"Failed to generate launcher: {result.stderr.decode()}")
        else:
            print("Launcher is up-to-date.")

        _cleanup()
    except Exception as ex:
        _cleanup()

        # Report exception.
        print(f"Error: {ex}")
        sys.exit(1)

if __name__ == "__main__":
    main()
