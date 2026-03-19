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

class VgaToneDataIdx:
    MODE             = 1  # `ToneMode`
    VOLUME           = 2  # `[0, 127]`
    PANNING          = 3  # `[0, 127]`
    CENTER           = 4  # `[0, 127]`
    SHIFT            = 5  # `[0, 127]`
    NOTE_MIN         = 6  # `[0, 127]`
    NOTE_MAX         = 7  # `[0, 127]`
    VIBRATO_WIDTH    = 8
    VIBRATO_TIME     = 9
    PORTAMENTO_WIDTH = 10
    PORTAMENTO_TIME  = 11
    PITCH_BEND_MIN   = 12
    PITCH_BEND_MAX   = 13
    ADSR1            = 16 # 2 bytes. Attack and decay.
    ADSR2            = 18 # 2 bytes. Sustain and release.
    VAG_ID           = 22 # 2 bytes.

class ToneMode:
    NORMAL = 0
    REVERB = 4

@dataclass
class VgaTone:
    program_idx:      int
    mode:             int
    volume:           int
    center:           int
    shift:            int
    panning:          int
    note_min:         int
    note_max:         int
    vibrato_width:    int
    vibrato_time:     int
    portamento_width: int
    portamento_time:  int
    pitch_bend_min:   int
    pitch_bend_max:   int
    adsr1:            int
    adsr2:            int
    vag_id:           int

################################3

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
    vag:       int

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

def _parse_vab(vab_file: Path):
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

def _psx_adsr_to_sec(rate, is_release=False):
    if rate <= 0: return 0.0
    # Refined SPU conversion: The PS1 uses a specific power-of-two curve.
    # Release 13 maps to ~0.512s in many SPU implementations.
    return round(0.001 * (2 ** (rate / 2)), 3) if not is_release else round(0.001 * (2 ** ((31 - rate) / 2)) * 32, 3)

def _build_sfz_from_vab(output_folder: Path, vab_file: Path):
    parsed_vab = _parse_vab(vab_file)
    if not parsed_vab: return

    header, programs = parsed_vab
    sfz_file = output_folder / vab_file.stem / f"{vab_file.stem}.sfz"

    with open(sfz_file, 'w') as output:
        for prog_idx, prog in enumerate(programs):
            for tone in prog.tones:
                wav_name = f"{SAMPLES_FOLDER}/{vab_file.stem}_{tone.vag}.WAV"

                # Now we use the center note directly from the VAB
                sf_center = tone.center 

                # Logarithmic volume conversion
                sfz_vol = round(20 * math.log10(max(tone.vol, 1) / 127), 2)
                
                # ADSR mapping
                ar = (tone.adsr1 >> 10) & 0x1F
                dr = (tone.adsr1 >> 6) & 0x0F
                sl = tone.adsr1 & 0x0F
                rr = tone.adsr2 & 0x1F

                region_data = {
                    "sample": wav_name,
                    "bank": 0,
                    "program": prog_idx,
                    "lokey": tone.min_note,
                    "hikey": tone.max_note,
                    "pitch_keycenter": sf_center,
                    "volume": sfz_vol,
                    "pan": round((tone.pan - 64) / 64 * 100),
                    "tune": round((tone.shift / 128) * 100),
                    "ampeg_attack": _psx_adsr_to_sec(ar),
                    "ampeg_decay": _psx_adsr_to_sec(dr),
                    "ampeg_sustain": round((sl / 15) * 100, 2),
                    "ampeg_release": _psx_adsr_to_sec(rr, is_release=True),
                }

                output.write("<region> ")
                output.write(" ".join([f"{k}={v}" for k, v in region_data.items()]))
                if tone.mode == 4: output.write(" loop_mode=loop_continuous")
                output.write("\n")

def _build_sfz_from_vab_OLD(output_folder: Path, vab_file: Path):
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

    with vab_file.open("rb") as _file:
        header = _file.read(32)
        magic, ver, vab_id, file_size, res0, prog_count, tone_count, vag_count = struct.unpack('<4sIIiHHHH', header[:24])

        if magic != b'pBAV':
            logging.error(f"Invalid `VAB`.")
            return

        # Collect programs.
        prog_tone_counts = []
        for i in range(128):
            prog_data  = _file.read(16)
            tone_count = prog_data[0]
            prog_tone_counts.append(tone_count)

        # Collect program tones.
        tones = []
        for i in range(prog_count):
            active_tones = prog_tone_counts[i]

            for tone_idx in range(16):
                raw_tone = _file.read(32)

                # Map tones of active programs.
                if active_tones > 0 and tone_idx < active_tones:
                    tone = VgaTone(
                        program_idx=i,
                        vag_id=struct.unpack('<h', raw_tone[VgaToneDataIdx.VAG_ID:VgaToneDataIdx.VAG_ID + 2])[0],
                        center=raw_tone[VgaToneDataIdx.CENTER],
                        note_min=raw_tone[VgaToneDataIdx.NOTE_MIN],
                        note_max=raw_tone[VgaToneDataIdx.NOTE_MAX],
                        vibrato_width=raw_tone[VgaToneDataIdx.VIBRATO_WIDTH],
                        vibrato_time=raw_tone[VgaToneDataIdx.VIBRATO_TIME],
                        portamento_width=raw_tone[VgaToneDataIdx.PORTAMENTO_WIDTH],
                        portamento_time=raw_tone[VgaToneDataIdx.PORTAMENTO_TIME],
                        pitch_bend_min=-raw_tone[VgaToneDataIdx.PITCH_BEND_MIN],
                        pitch_bend_max=raw_tone[VgaToneDataIdx.PITCH_BEND_MAX],
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
                # Extract values
                ar = (tone.adsr1 >> 10) & 0x1F
                dr = (tone.adsr1 >> 4)  & 0x0F
                sl = tone.adsr1         & 0x0F
                sr = (tone.adsr2 >> 6)  & 0x7F
                rr = tone.adsr2         & 0x1F

                sfz_vibrato_freq    = (tone.vibrato_time / 127) * 10    # VAB `[0, 127]` -> SFZ hertz.
                sfz_vibrato_depth   = (tone.vibrato_width / 127) * 100  # VAB `[0, 127]` -> SFZ cents.
                sfz_portamento_time = (tone.portamento_time / 127) * 0.5 # Max 0.5 seconds glide.
                # @todo portamento width?
                sfz_bend_up         = tone.pitch_bend_max * 100
                sfz_bend_down       = tone.pitch_bend_min * 100
                sfz_pan             = round((tone.panning - 64) / 64 * 100) # VAB (0..64..127) -> SFZ (-100..0..100)
                sfz_fine_tune       = round((tone.shift / 128) * 100)       # VAB `[0, 127]` -> SFZ cents.
                sfz_loop            = "loop_mode=loop_continuous " if tone.mode == ToneMode.REVERB else ""
                sfz_attack          = convert_psx_rate_to_sec((tone.adsr1 >> 10) & 0x1F, 0x1F)
                sfz_decay           = convert_psx_rate_to_sec(dr, 0x0F)
                sfz_sustain         = convert_psx_sl_to_percent(sl)
                sfz_decay2          = f"ampeg_decay2={convert_psx_rate_to_sec(sr, 0x7F)} ampeg_sustain2=0 " if sr > 0 else ""
                sfz_release         = convert_psx_rate_to_sec(rr, 0x1F)

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
                    #f"pitchlfo_freq={sfz_vibrato_freq} "
                    #f"pitchlfo_depth={sfz_vibrato_depth} "
                    #f"pitchlfo_delay=0.1 " # Adding a tiny delay makes it sound more natural
                    #f"oppua_portamento={sfz_portamento_time} "
                    f"bend_up={sfz_bend_up} "
                    f"bend_down={sfz_bend_down} "
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
            "-l", "1.0",
            "-f", "0.0",
            "-o", str(wav_file),
            str(vab_file)
        ]
        result = subprocess.run(command)
        
        # Patch the header so it plays at 44.1k natively.
        _patch_wav_rate(wav_file)

        # Report status.
        if result.returncode != 0:
            logging.error(f"Failed to extract sample {i}.")

def main():
    #try:
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
            #subfolder.mkdir(parents=True, exist_ok=True)

            #_convert_kdt_to_midi(args.kdtToolScript, args.outputFolder, args.kdtFile)

        # Process `VAB`.
        if args.vabFile:
            # Create subfolders.
            subfolder = args.outputFolder / args.vabFile.stem / SAMPLES_FOLDER
            subfolder.mkdir(parents=True, exist_ok=True)

            _build_sfz_from_vab(args.outputFolder, args.vabFile)
            _extract_vab_samples_to_wav(args.vgmstreamExe, args.outputFolder, args.vabFile)
    #except Exception as ex:
    #    logging.error(f"{ex}")
    #    sys.exit(1)

if __name__ == "__main__":
    main()
