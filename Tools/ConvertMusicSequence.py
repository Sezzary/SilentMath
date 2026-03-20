"""
Music sequence converter.

Converts a `KDT` sequence to `MIDI` and a `VAB` file to `SFZ`+`WAV`s.
"""

import logging
import math
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
VAG_PREFIX     = "VAG_"

# VAB header.
@dataclass
class VabHeader:
    magic:      bytes
    version:    int
    vab_id:     int
    file_size:  int
    reserved0:  int
    prog_count: int
    tone_count: int
    vag_count:  int
    master_vol: int
    master_pan: int
    attr1:      int
    attr2:      int
    reserved1:  int

# VAB tone attributes.
@dataclass
class ToneAttr:
    priority:  int
    mode:      int
    vol:       int
    pan:       int
    center:    int
    shift:     int
    min_note:  int
    max_note:  int
    vib_w:     int
    vib_t:     int
    por_w:     int
    por_t:     int
    pb_min:    int
    pb_max:    int
    reserved1: int
    reserved2: int
    adsr1:     int
    adsr2:     int
    prog:      int
    vag_id:    int

# VAB program attributes.
@dataclass
class ProgramAttr:
    tone_count: int
    vol:        int
    priority:   int
    mode:       int
    pan:        int
    reserved0:  int
    attr:       int
    reserved1:  int
    reserved2:  int
    tones:      list

def _create_parser():
    """
    Create an argument parser for the script.
    """
    parser = ArgumentParser()
    parser.add_argument("--vgmstreamExe", "-exe", type=Path, help="Path to the `vgmstream-cli` executable (`VAB` -> `WAV`s tool).")
    parser.add_argument("--kdtToolPy", "-ktp", type=Path, help="Path to the `kdt-tool.py` script (`KDT` -> `MIDI` tool).")
    parser.add_argument("--convertSoundBankPy", "-csp", type=Path, help="Path to the `convertSoundBank.py` script (`SFZ` -> `SF2` tool).")
    parser.add_argument("--kdtFile", "-ikf", type=Path, help="Path to the optional `KDT` file.")
    parser.add_argument("--vabFile", "-ivf", type=Path, help="Path to the optional `VAB` file.")
    parser.add_argument("outputFolder", type=Path, help="Path to the folder where converted `MIDI`, `SFZ`+`WAV`s, and `SF2` will be saved.")
    return parser

def _get_python_cmd():
    """
    Get the platform-specific system Python command.
    """
    system_os = platform.system().lower()
    return "python" if system_os == "windows" else "python3"

def _convert_kdt_to_midi(kdt_tool_py: Path, output_folder: Path, kdt_file: Path):
    """
    Convert a `KDT` sequence to `MIDI`.

    :param kdt_tool_py: Path to the `kdt-tool.py` script.
    :param output_folder: Directory where the `MIDI` will be saved.
    :param file: The source `KDT` file to convert.
    """
    logging.info(f"Converting `{kdt_file.name}` to `MIDI`...")

    # Run command.
    python_cmd = _get_python_cmd()
    command    = [
        python_cmd,
        kdt_tool_py,
        "-c", kdt_file
    ]
    result = subprocess.run(command, capture_output=True, text=True)

    # Report status.
    if result.returncode != 0:
        logging.error(f"Conversion failed.")
        return

    # Move `MIDI` to subfolder.
    midi_file = f"./{kdt_file.stem}.midi"
    shutil.move(midi_file, output_folder / kdt_file.stem) # @todo overwrite.

def _extract_vab_samples_to_wav(vgmstream_exe: Path, output_folder: Path, vab_file: Path):
    """
    Extract audio samples from a `VAB` as `WAV`.
    Each sample file's name takes the stem of the parent `VAB` and appends a numeric suffix as `_*`.
    Indexing is 1-based.

    :param vgmstream_exe: Path to the `vgmstream-cli` executable.
    :param output_folder: Directory where the `WAV` samples will be saved.
    :param vab_file: The source `VAB` file to process.
    """
    def _patch_wav_rate(wav_path: Path):
        """
        Overwrite a `WAV`'s header sample rate 44100 to without re-encoding data.

        :param wav_path: The source `WAV` file to process.
        """
        RATE = 44100

        if not wav_path.exists(): return
        with open(wav_path, "r+b") as _file:
            _file.seek(24)                           # Position of `SampleRate` in RIFF `WAV` header.
            _file.write(struct.pack("<I", RATE))
            _file.seek(28)                           # Position of `ByteRate`.
            _file.write(struct.pack("<I", RATE * 2)) # Assuming 16-bit mono.

    logging.info(f"Extracting samples from `{vab_file.name}` as `WAV`...")

    # Run metadata fetch command.
    command = [
        vgmstream_exe.resolve(),
        "-m", vab_file
    ]
    result = subprocess.run(command, capture_output=True, text=True)

    # Get stream count.
    match = re.search(r"stream count: (\d+)", result.stdout)
    if not match:
        logging.error(f"Failed to determine stream count.")
        return
    stream_count = int(match.group(1))

    # Run through samples.
    logging.info(f"{stream_count} samples found.")
    for i in range(1, stream_count):
        logging.info(f"Extracting sample {i}...")

        # Run conversion command.
        wav_file = output_folder / vab_file.stem / SAMPLES_FOLDER / f"{VAG_PREFIX}{i:03}.WAV"
        command = [
            vgmstream_exe.resolve(),
            "-s", str(i),
            "-l", "1.0",
            "-f", "0.0",
            "-L",
            "-o", str(wav_file),
            str(vab_file)
        ]
        result = subprocess.run(command)
        
        # Patch the header so it plays at 44.1k natively.
        _patch_wav_rate(wav_file)

        # Report status.
        if result.returncode != 0:
            logging.error(f"Failed to extract sample {i}.")

def _parse_vab(vab_file: Path):
    """
    Parse a `VAB` file into a readable header and programs.
    
    :param vab_file: The source `VAB` file to process.
    """
    logging.info(f"Parsing `{vab_file.name}`...")

    with vab_file.open("rb") as _file:
        header_data     = _file.read(32)
        header_unpacked = struct.unpack('<4sIIIHHHHBBBBI', header_data)
        header          = VabHeader(*header_unpacked)

        if header.magic != b'pBAV':
            logging.error("Invalid `VAB`.")
            return None

        # 128 fixed programs.
        programs = []
        for _ in range(128):
            prog_data     = _file.read(16)
            prog_unpacked = struct.unpack('<BBBBBBHII', prog_data)
            prog          = ProgramAttr(*prog_unpacked, tones=[])
            programs.append(prog)

        # 16 tones per program.
        for i in range(header.prog_count):
            for j in range(16):
                tone_data = _file.read(32)

                if programs[i].tone_count > 0 and j < programs[i].tone_count:
                    tone_unpacked = struct.unpack('<BBBBBBBBBBBBBBBBHHhh8s', tone_data)
                    tone          = ToneAttr(*tone_unpacked[:-1])
                    programs[i].tones.append(tone)

        return header, programs

def _build_sfz_from_vab(output_folder: Path, vab_file: Path):
    """
    Build an `SFZ` from a `VAB`.

    :param output_folder: Directory where the `SFZ` will be saved.
    :param vab_file: The source `VAB` to convert.
    """
    def get_wav_loop_points(wav_file: Path):
        """
        Extract the loop start/end points from the `smpl` chunk of a `WAV`. If loop points don't exist, it returns the
        start and end of the sample as the loop points.

        :param wav_file: The source `WAV` to process.
        :return: The start/end loop points and a flag noting if the `WAV` has a loop.
        """
        with open(wav_file, "rb") as _file:
            _file.read(12) # Skip `RIFF` header.

            total_samples = 0
            while True:
                try:
                    chunk_id, chunk_size = struct.unpack("<4sI", _file.read(8))

                    # Fallback: calculate total samples from `data` chunk.
                    if chunk_id == b"data":
                        total_samples = chunk_size // 2

                    if chunk_id == b"smpl":
                        _file.read(28) # Skip standard `smpl` header data.
                        loopCount = struct.unpack("<I", _file.read(4))[0]
                        _file.read(4) # Skip sampler data.
                        if loopCount > 0:
                            _file.read(8) # Skip cue point ID and type.
                            start, end = struct.unpack("<II", _file.read(8))
                            return start, end, True
                        
                    _file.seek(chunk_size if chunk_size % 2 == 0 else chunk_size + 1, 1)
                except Exception:
                    break

        return 0, max(0, total_samples - 1), False

    logging.info(f"Building `SFZ` from `{vab_file.name}`...")

    parsed_vab = _parse_vab(vab_file)
    if not parsed_vab:
        return

    header, progs = parsed_vab
    sfz_file      = output_folder / vab_file.stem / f"{vab_file.stem}.sfz"

    with open(sfz_file, 'w') as output:
        for prog_idx, prog in enumerate(progs):
            for tone in prog.tones:
                wav_name = f"{SAMPLES_FOLDER}/{VAG_PREFIX}{tone.vag_id:03}.WAV"

                # ADSR mapping.
                ar = (tone.adsr1 >> 8) & 0x7F # Attack        | 7 bits (0-127).
                dr = (tone.adsr1 >> 4) & 0x0F # Decay         | 4 bits (0-15).
                sl = tone.adsr1 & 0x0F        # Sustain level | 4 bits (0-15).
                rr = tone.adsr2 & 0x1F        # Release       | 5 bits (0-31).

                # Get sample loop points.
                wav_file                            = output_folder / vab_file.stem / wav_name
                sample_start, sample_end, is_looped = get_wav_loop_points(wav_file)

                # Write program header.
                output.write("<group> ")
                group_data = {
                    f"bank":    0,
                    f"program": prog_idx
                }
                output.write(" ".join([f"{key}={value}" for key, value in group_data.items()]))
                output.write("\n")

                # Write sample header.
                output.write("<region> ")
                region_data = {
                    "sample":          wav_name,
                    "bank":            0,
                    "program":         prog_idx,
                    #"offset":          28, @todo `WAV`s have 28 samples of padding at the start.
                    "loop_mode":       "loop_continuous" if is_looped else "one_shot",
                    "loop_start":      sample_start,
                    "loop_end":        sample_end,
                    "lokey":           tone.min_note,
                    "hikey":           tone.max_note,
                    "pitch_keycenter": tone.center,
                    "volume":          round(20 * math.log10(max(tone.vol, 1) / 127), 2),
                    "pan":             round(((tone.pan - 64) / 64) * 100),
                    "tune":            round((tone.shift / 128) * 100),
                    "ampeg_attack":    0.0 if ar == 0  else round(0.001 * (2 ** ((127 - ar) / 9.6)), 3),
                    "ampeg_decay":     0.0 if sl == 15 else round(0.001 * (2 ** (dr / 4)), 3),
                    "ampeg_sustain":   round((sl / 15) * 100, 2),
                    "ampeg_release":   round(min(0.000148 * (2 ** rr), 5.0), 3),
                    "bend_up":         tone.pb_max * 100,
                    "bend_down":       tone.pb_min * 100,
                    "pitchlfo_freq":   round((tone.vib_t / 127) * 10, 2),
                    "pitchlfo_depth":  round((tone.vib_w / 127) * 50, 2),
                }
                output.write(" ".join([f"{key}={value}" for key, value in region_data.items()]))
                output.write("\n\n")

def _convert_sfz_to_sf2(convert_sound_bank_py: Path, output_folder: Path, sfz_file: Path):
    """
    Convert an `SFZ` file to `SF2`.

    :param convert_sound_bank_py: Path to the `convertSoundBank.py` script.
    :param output_folder: Directory where the `SF2` file will be saved.
    :param sfz_file: The source `SFZ` file to process.
    """
    logging.info(f"Converting `{sfz_file.name}` to `SF2`...")

    # Run command.
    python_cmd = _get_python_cmd()
    command    = [
        python_cmd,
        convert_sound_bank_py.resolve(),
        sfz_file,
        output_folder / sfz_file.stem / f"{sfz_file.stem}.sf2"
    ]
    result = subprocess.run(command)

    # Report status.
    if result.returncode != 0:
        logging.error(f"Conversion failed.")

def _cleanup():
    """
    Clean up process files.
    """
    # @todo

def main():
    try:
        # Setup.
        logging.basicConfig(level = logging.INFO)

        parser = _create_parser()
        args   = parser.parse_args()
        if not args.kdtFile and not args.vabFile:
            parser.error("At least one input file (`--kdtFile` or `--vabFile`) must be provided.")

        # Process `KDT` -> `MIDI`.
        if args.kdtFile:
            subfolder = args.outputFolder / args.kdtFile.stem
            subfolder.mkdir(parents=True, exist_ok=True)

            _convert_kdt_to_midi(args.kdtToolPy, args.outputFolder, args.kdtFile)

        # Process `VAB` -> `SFZ`+`WAV`s -> `SF2`.
        if args.vabFile:
            subfolder = args.outputFolder / args.vabFile.stem / SAMPLES_FOLDER
            subfolder.mkdir(parents=True, exist_ok=True)

            _extract_vab_samples_to_wav(args.vgmstreamExe, args.outputFolder, args.vabFile)
            _build_sfz_from_vab(args.outputFolder, args.vabFile)

            sfz_file = args.outputFolder / args.vabFile.stem / f"{args.vabFile.stem}.sfz"
            if args.convertSoundBankPy and sfz_file.exists():
                _convert_sfz_to_sf2(args.convertSoundBankPy, args.outputFolder, sfz_file)

        _cleanup()
    except Exception as ex:
        _cleanup()

        logging.error(f"{ex}")
        sys.exit(1)

if __name__ == "__main__":
    main()
