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

LAUNCHER_NAME      = "Launcher.py"
SPEC_NAME          = "Launcher.spec"
BASE_PATH          = Path(__file__).parent
BUILD_PATH         = BASE_PATH / "../Build"
DEBUG_BUILD_PATH   = BUILD_PATH / "Debug"
RELEASE_BUILD_PATH = BUILD_PATH / "Release"
TEMP_OUTPUT_PATH   = BUILD_PATH / ".temp"
LAUNCHER_SCRIPT    = BASE_PATH / LAUNCHER_NAME
SPEC_FILE          = BUILD_PATH / SPEC_NAME

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
        platform_name        = _get_platform_name()
        exe_ext              = ".exe" if platform_name == "Windows" else ""
        colon                = ";"    if platform_name == "Windows" else ":"
        launcher_exes        = [
            DEBUG_BUILD_PATH   / f"Launcher{exe_ext}",
            RELEASE_BUILD_PATH / f"Launcher{exe_ext}"
        ]

        # Check if new launcher build is required.
        existing_mtimes = [
            os.path.getmtime(file) for file in launcher_exes
            if os.path.exists(file)
        ]
        newest_exe_mtime = max(existing_mtimes, default=0)
        run_new_build    = os.path.getmtime(LAUNCHER_SCRIPT) > newest_exe_mtime

        # Run generation command.
        if run_new_build:
            command = ["pyinstaller", "--onefile", "--windowed", "--noconfirm",
                       "--add-data", BASE_PATH / f"ExtractAssets.py{colon}.",
                       "--add-binary", BASE_PATH / f"dumpsxiso/{platform_name}/dumpsxiso{exe_ext}{colon}.",
                       "--distpath", BUILD_PATH,
                       "--workpath", TEMP_OUTPUT_PATH,
                       "--specpath", BUILD_PATH,
                       BASE_PATH / "Launcher.py"]
            result  = subprocess.run(command)

            # Report status and copy launcher to final output folders.
            if result.returncode == 0:
                DEBUG_BUILD_PATH.mkdir(parents=True, exist_ok=True)
                RELEASE_BUILD_PATH.mkdir(parents=True, exist_ok=True)

                shutil.copy2(BUILD_PATH / f"Launcher{exe_ext}", DEBUG_BUILD_PATH)
                shutil.copy2(BUILD_PATH / f"Launcher{exe_ext}", RELEASE_BUILD_PATH)

                print("Launcher generated successfully.")
            else:
                raise Exception(f"Failed to generate launcher: {result.stderr.decode()}")
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
