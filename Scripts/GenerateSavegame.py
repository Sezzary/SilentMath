import os
import platform
import subprocess
import sys
from pathlib import Path

FLATC_NAME   = "flatc"
BASE_PATH    = Path(__file__).resolve().parent
FLATC_PATH   = BASE_PATH / "Tools" / FLATC_NAME
SCHEMAS_PATH = BASE_PATH / "../Source/Engine/Services/Savegame/Schemas"
OUTPUT_PATH  = BASE_PATH / "../Source/Engine/Services/Savegame/Generated"

"""
Get the path to the appropriate `flatc` executable based on the system OS.
"""
def get_flatc_executable():
    system_os = platform.system()

    # Define OS-specific executable path.
    flatc_exe = Path()
    if system_os == "Windows":
        flatc_exe = os.path.join(FLATC_PATH, "Windows", FLATC_NAME + ".exe")
    elif system_os == "Linux":
        flatc_exe = os.path.join(FLATC_PATH, "Linux", FLATC_NAME)
    elif system_os == "Darwin": # MacOS.
        flatc_exe = os.path.join(FLATC_PATH, "MacOs", FLATC_NAME)
    else:
        raise Exception(f"Unsupported OS '{system_os}'.")

    if not os.path.isfile(flatc_exe):
        raise Exception(f"`flatc` executable not found at '{flatc_exe}'.")

    return flatc_exe

"""
Run `flatc` to generate FlatBuffers headers.
"""
def generate_savegame_headers():
    try:
        print("Generating savegame FlatBuffers headers...")

        flatc_exe = get_flatc_executable()

        # Process schemas.
        schemas = ["savegame.fbs"]
        for schema in schemas:
            print(f"Processing `{schema}`...")
            os.makedirs(OUTPUT_PATH, exist_ok=True)

            # TODO: Setting output path doesn't work. For now, headers generate in project root and must be moved manually.
            # Build command.
            schema_path = os.path.join(SCHEMAS_PATH, schema)
            command     = [ flatc_exe, "--cpp", schema_path ]#, "--output", str(OUTPUT_PATH) ]

            # Run generation command.
            result = subprocess.run(command, capture_output=True)
            if result.returncode != 0:
                print(f"Error running `flatc`: {result.stderr.decode()}")
                sys.exit(result.returncode)

        print("Savegame FlatBuffers header generation complete.")
    except Exception as ex:
        print(f"Error: {ex}")
        sys.exit(1)

if __name__ == "__main__":
    generate_savegame_headers()
