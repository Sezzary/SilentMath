"""
Music sequence converter.

Converts a `KDT` sequence to `MIDI` and a `VAB` file to `SFZ`+`WAV`s.
"""

import logging
import platform
import re
import shutil
import struct
import subprocess
import sys

from argparse    import ArgumentParser
from dataclasses import dataclass
from pathlib     import Path

SAMPLES_FOLDER = "Samples"

class VgaToneDataIdx:
    MODE         = 1  # 0 = normal, 4 = reverb.
    VOLUME       = 2  # `[0, 127]`
    PANNING      = 3  # `[0, 127]`
    CENTER       = 4  # `[0, 127]`
    SHIFT        = 5  # `[0, 127]`
    NOTE_MIN     = 6  # `[0, 127]`
    NOTE_MAX     = 7  # `[0, 127]`
    ADSR1        = 16 # 2 bytes. Attack and decay.
    ADSR2        = 18 # 2 bytes. Sustain and release.
    VAG_ID       = 22 # 2 bytes.

@dataclass
class VgaTone:
    program_idx: int
    mode:        int
    volume:      int
    center:      int
    shift:       int
    panning:     int
    note_min:    int
    note_max:    int
    adsr1:       int
    adsr2:       int
    vag_id:      int

def _create_parser():
    """
    Create an argument parser for the script.
    """
    parser = ArgumentParser()
    parser.add_argument("--vgmstreamExe", "-exe", type=Path, help="Path to the `vgmstream-cli` executable.")
    parser.add_argument("--kdtToolScript", "-scr", type=Path, help="Path to the `kdt-tool.py` script.")
    parser.add_argument("--kdtFile", "-ikf", type=Path, help="Path to the optional `KDT` file.")
    parser.add_argument("--vabFile", "-ivf", type=Path, help="Path to the optional `VAB` file.")
    parser.add_argument("outputFolder", type=Path, help="Path to the folder where converted `MIDI` and `SFZ`+`WAV` files will be saved.")
    return parser

def _get_python_cmd():
    """
    Get the platform-specific system Python command.
    """
    system_os = platform.system().lower()
    return "python" if system_os == "windows" else "python3"

def _convert_kdt_to_midi(kdt_tool_script: Path, output_folder: Path, kdt_file: Path):
    """
    Convert a `KDT` sequence to `MIDI`.

    :param kdt_tool_script: Path to the `kdt-tool.py` script.
    :param output_folder: Directory where the `MIDI` will be saved.
    :param file: The source `KDT` to convert.
    """
    logging.info(f"Converting `{kdt_file.name}` to `MIDI`...")

    # Run command.
    python_cmd = _get_python_cmd()
    command    = [
        python_cmd,
        kdt_tool_script,
        "-c", kdt_file
    ]
    result = subprocess.run(command, capture_output=True, text=True)

    # Report status.
    if result.returncode != 0:
        logging.error(f"Failed to convert `{kdt_file.name}`.")
        return

    # Move `MIDI` to subfolder.
    midi_file = f"./{kdt_file.stem}.midi"
    shutil.move(midi_file, output_folder / kdt_file.stem) # @todo overwrite.

def _build_sfz_from_vab(output_folder: Path, vab_file: Path):
    """
    Build an `SFZ` from a `VAB`.

    :param output_folder: Directory where the `SFZ` will be saved.
    :param vab_file: The source `VAB` to convert.
    """
    def convert_psx_rate_to_sec(rate, max_val=0x1F):
        """
        Approximate a PSX SPU rate to `SFZ` seconds.

        :param rate: PSX SPU rate to convert.
        """
        if rate == 0:
            return 0.0

        # Higher rate = faster time. This creates a usable curve.
        return round(0.001 * (2 ** ((max_val - rate) / 2)), 3)

    def convert_psx_sl_to_percent(sustain):
        """
        Convert a 4-bit sustain level `[0, 15]` to `SFZ` percentage `[0, 100]`.

        :param sustain: PSX sustain level to convert.
        """
        return round((sustain / 15) * 100, 2)

    logging.info(f"Building `SFZ` from `{vab_file.name}`...")

    with vab_file.open("rb") as f:
        header = f.read(32)
        magic, ver, vab_id, file_size, res0, prog_count, tone_count, vag_count = struct.unpack('<4sIIiHHHH', header[:24])

        if magic != b'pBAV':
            logging.error(f"Invalid `VAB`.")
            return

        # Collect programs. 128 16-byte programs.
        progs = []
        for i in range(128):
            prog_data  = f.read(16)
            tone_count = prog_data[0]
            progs.append(tone_count)

        # Collect tones. 16 tones per 128 32-byte programs.
        tones = []
        for prog_idx in range(prog_count):
            for tone_idx in range(16):
                raw_tone = f.read(32)

                # Map tones of active programs.
                if progs[prog_idx] > 0 and tone_idx < progs[prog_idx]:
                    tone = VgaTone(
                        program_idx=prog_idx,
                        vag_id=struct.unpack('<h', raw_tone[VgaToneDataIdx.VAG_ID:VgaToneDataIdx.VAG_ID + 2])[0],
                        center=raw_tone[VgaToneDataIdx.CENTER],
                        note_min=raw_tone[VgaToneDataIdx.NOTE_MIN],
                        note_max=raw_tone[VgaToneDataIdx.NOTE_MAX],
                        panning=raw_tone[VgaToneDataIdx.PANNING],
                        shift=raw_tone[VgaToneDataIdx.SHIFT],
                        volume=raw_tone[VgaToneDataIdx.VOLUME],
                        mode=raw_tone[VgaToneDataIdx.MODE],
                        adsr1=struct.unpack('<H', raw_tone[VgaToneDataIdx.ADSR1:VgaToneDataIdx.ADSR1 + 2])[0],
                        adsr2=struct.unpack('<H', raw_tone[VgaToneDataIdx.ADSR2:VgaToneDataIdx.ADSR2 + 2])[0]
                    )
                    tones.append(tone)

        # Write tones.
        sfz_file = output_folder / vab_file.stem / f"{vab_file.stem}.SFZ"
        with open(sfz_file, 'w') as output:
            for tone in tones:
                # Extract values.
                ar = (tone.adsr1 >> 10) & 0x1F
                dr = (tone.adsr1 >> 4)  & 0x0F
                sl = tone.adsr1         & 0x0F
                sr = (tone.adsr2 >> 6)  & 0x7F
                rr = tone.adsr2         & 0x1F

                sfz_pan       = round((tone.panning - 64) / 64 * 100) # VAB (0..64..127) -> SFZ (-100..0..100)
                sfz_fine_tune = round((tone.shift / 128) * 100) # VAB `[0, 127]` -> SFZ cents
                sfz_loop      = "loop_mode=loop_continuous " if tone.mode == 4 else ""
                sfz_attack    = convert_psx_rate_to_sec((tone.adsr1 >> 10) & 0x1F, 0x1F)
                sfz_decay     = convert_psx_rate_to_sec(dr, 0x0F)
                sfz_sustain   = convert_psx_sl_to_percent(sl)
                sfz_decay2    = f"ampeg_decay2={convert_psx_rate_to_sec(sr, 0x7F)} ampeg_sustain2=0 " if sr > 0 else ""
                sfz_release   = convert_psx_rate_to_sec(rr, 0x1F)

                # @todo
                # Handle the 'Sustain Rate' (SR)
                # If SR is 0, the note holds forever until release. 
                # If SR > 0, it slowly fades out during the hold.
                #sfz_hold_decay = _psx_rate_to_sec(sr, 0x7F) if sr > 0 else 100.0
                # If SR > 0, the sound fades toward silence (0%) during the sustain phase.

                # Write SFZ data.
                wav_name = f"{SAMPLES_FOLDER}/{vab_file.stem}_{tone.vag_id}.WAV"
                output.write(
                    f"<region> "
                    f"sample={wav_name} "
                    f"bank=0 "
                    f"program={tone.program_idx} "
                    f"lokey={tone.note_min} "
                    f"hikey={tone.note_max} "
                    f"pitch_keycenter={tone.center} "
                    f"volume={tone.volume} "
                    f"pan={sfz_pan} "
                    f"tune={sfz_fine_tune} "
                    #f"ampeg_attack={sfz_attack} "
                    #f"ampeg_decay={sfz_decay} "
                    #f"ampeg_sustain={sfz_sustain} "
                    #f"{sfz_decay2}"
                    #f"ampeg_release={sfz_release} "
                    f"{sfz_loop}\n"
                )

def _extract_vab_samples_to_wav(vgmstream_exe: Path, output_folder: Path, vab_file: Path):
    """
    Extract audio samples from a `VAB` as `WAV`.
    Each sample file's name takes the stem of the parent `VAB` and appends a numeric suffix as `_*`.
    Indexing is 1-based.

    :param vgmstream_exe: Path to the `vgmstream-cli` executable.
    :param output_folder: Directory where the `WAV` samples will be saved.
    :param vab_file: The source `VAB` file to process.
    """
    # Run metadata fetch command.
    command = [
        vgmstream_exe.resolve(),
        "-m", vab_file
    ]
    result = subprocess.run(command, capture_output=True, text=True)

    # Get stream count.
    match = re.search(r"stream count: (\d+)", result.stdout)
    if not match:
        logging.error(f"Failed to determine stream count for `{vab_file.name}`.")
        return
    stream_count = int(match.group(1))

    logging.info(f"Extracting {stream_count} samples from `{vab_file.name}` as `WAV`...")

    # Run through samples.
    logging.info(f"Processing `{vab_file.name}` samples...")
    for i in range(1, stream_count):
        logging.info(f"Extracting sample {i} as `WAV`...")

        # Run conversion command.
        wav_file = output_folder / vab_file.stem / SAMPLES_FOLDER / f"{vab_file.stem}_{i}.WAV"
        command = [
            vgmstream_exe.resolve(),
            "-s", str(i),
            "-o", wav_file,
            vab_file
        ]
        result = subprocess.run(command)

        # Report status.
        if result.returncode != 0:
            logging.error(f"Failed to extract sample {i}.")

def main():
    try:
        # Setup.
        logging.basicConfig(level = logging.INFO)

        parser = _create_parser()
        args   = parser.parse_args()
        if not args.kdtFile and not args.vabFile:
            parser.error("At least one input file (`--kdtFile` or `--vabFile`) must be provided.")

        # Process `KDT`.
        if args.kdtFile:
            # Create subfolder.
            subfolder = args.outputFolder / args.kdtFile.stem
            subfolder.mkdir(parents=True, exist_ok=True)

            _convert_kdt_to_midi(args.kdtToolScript, args.outputFolder, args.kdtFile)

        # Process `VAB`.
        if args.vabFile:
            # Create subfolders.
            subfolder = args.outputFolder / args.vabFile.stem / SAMPLES_FOLDER
            subfolder.mkdir(parents=True, exist_ok=True)

            _build_sfz_from_vab(args.outputFolder, args.vabFile)
            _extract_vab_samples_to_wav(args.vgmstreamExe, args.outputFolder, args.vabFile)
    except Exception as ex:
        logging.error(f"{ex}")
        sys.exit(1)

if __name__ == "__main__":
    main()
