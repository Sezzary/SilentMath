
"""
Audio and video converted

Converts `.XA` (audio) and `.STR` (video) asset files to usable formats.
"""

import shutil
import subprocess
import sys

from argparse      import ArgumentParser
from pathlib       import Path
from static_ffmpeg import run

def _create_parser():
    """
    Create an argument parser for the script.
    """
    parser = ArgumentParser()
    parser.add_argument("inputXaStrFolder", type = Path, help = "Path to the folder containing .XA and .STR files.")
    parser.add_argument("outputAudioFolder", type = Path, help = "Path to the folder where processed audio will be saved.")
    parser.add_argument("outputVideoFolder", type = Path, help = "Path to the folder where processed video will be saved.")
    return parser

def _get_ffmpeg_cmd():
    """
    Get the platform-specific system FFmpeg command.
    If FFmpeg is missing, it will fall back on a static download.
    """
    ffmpeg_exe = shutil.which("ffmpeg")
    if ffmpeg_exe:
        return ffmpeg_exe

    ffmpeg_exe = run.get_or_fetch_platform_executables_else_raise()
    return ffmpeg_exe

def _convert_xa(ffmpeg_cmd: str, outputFolder: Path, file: Path):
    """
    Convert an .XA audio file to .WAV.
    """
    newFile = f"{file.stem}.WAV"
    print(f"Converting `{file.name}` to `{newFile}`...")

    # Run command.
    command = [
        ffmpeg_cmd, "-y",
        "-hide_banner",
        "-loglevel", "error",
        "-i", str(file),
        outputFolder / newFile
    ]
    result = subprocess.run(command)

    # Report status.
    if result.returncode != 0:
        raise Exception(f".XA asset conversion failed for file `{file.name}`: {result.stderr.decode()}")

def _convert_str(ffmpeg_cmd: str, outputFolder: Path, file: Path):
    """
    Convert an .STR video to .MPG.
    """
    newFile = f"{file.stem}.MPG"
    print(f"Converting `{file.name}` to `{newFile}`...")

    # Run command.
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
        outputFolder / newFile
    ]
    result = subprocess.run(command)

    # Report status.
    if result.returncode != 0:
        raise Exception(f".STR asset conversion failed for file `{file.name}`: {result.stderr.decode()}")

def main():
    try:
        print("Converting audio and video...")

        # Setup.
        args       = _create_parser().parse_args()
        ffmpeg_cmd = _get_ffmpeg_cmd()

        # Create folders.
        args.outputAudioFolder.mkdir(parents=True, exist_ok=True)
        args.outputVideoFolder.mkdir(parents=True, exist_ok=True)

        # Run through `.XA` and `.STR` files.
        for file in args.inputXaStrFolder.iterdir():
            if file.suffix == ".XA":
                _convert_xa(ffmpeg_cmd, args.outputAudioFolder, file)
            elif file.suffix == ".STR":
                _convert_str(ffmpeg_cmd, args.outputVideoFolder, file)
            else:
                continue

        print("Audio and video conversion complete.")
    except Exception as ex:
        # Report exception.
        print(f"Error: {ex}")
        sys.exit(1)

if __name__ == "__main__":
    main()
