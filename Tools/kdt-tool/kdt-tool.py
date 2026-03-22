"""
Source: https://github.com/Nisto/kdt-tool
Modified by Sezz, 2026

kdt-tool

General-purpose tool for Konami's sequenced music format "KDT1" allowing you to:
- Convert a file to MIDI
- Dump event data in a human-readable format

Usage
- Convert to MIDI: `python3 kdt-tool.py -c SOURCE.KDT`
- Dump event data: `python3 kdt-tool.py -l SOURCE.KDT`

Known issues
MIDI conversion is not perfect, but it will get you notes, timing, track configuration and the most basic commands
converted. The remaining events which needs support is likely a subset stemming from Sony's SEQp format and should be
relatively easy to add hopefully, but I have not had the time to really look into it.

So far I have only tested the script with Silent Hill and Suikoden 2. There may be issues in calculating accurate tempos
for other games that use sequence command 0xC7 to set the tempo, as this command appears to be using a
(console-specific?) calculation to compensate for some kind of timing delay(?) Both Silent Hill and Suikoden 2
multiplies the tempo parameter by 2 and adds 2 (e.g. (29 * 2) + 2 = 60). However, using this equation (even for Silent
Hill or Suikoden 2 themselves), the tempo will still sound off when converted to a standard MIDI and played back on any
modern PC. In the KCET driver for Silent Hill 2 (PlayStation 2), the equation was changed to ((x * 2) + 10), which
appears to give a more accurate real-world tempo, so I've decided to use it universally. If needed, change it in the
sources and report your results please. Thanks!
"""

import os
import sys
import struct

class KDT:

    SIZE_LIMIT = 50*1024*1024

    HEADER_SIZE = 0x10

    OFF_FILE_SIZE   = 0x04
    OFF_TICKDIV     = 0x08
    OFF_TRACK_COUNT = 0x0C
    OFF_TRACK_SIZES = 0x10

    def __init__(self, path, log=False, convert=False):
        self.path = path
        self.log = log
        self.convert = convert

        if os.path.getsize(self.path) > KDT.SIZE_LIMIT:
            sys.exit("ERROR: File too large: %s" % self.path)

        with open(self.path, "rb") as kdt:
            self.buf = kdt.read()

        if self.buf[:4] != b"KDT1" or os.path.getsize(self.path) < KDT.HEADER_SIZE:
            sys.exit("ERROR: Not a valid KDT1 file: %s" % self.path)

        self.filesize = struct.unpack("<I", self.buf[KDT.OFF_FILE_SIZE:KDT.OFF_FILE_SIZE+4])[0]
        self.tickdiv = struct.unpack("<H", self.buf[KDT.OFF_TICKDIV:KDT.OFF_TICKDIV+2])[0]
        self.tracks = struct.unpack("<H", self.buf[KDT.OFF_TRACK_COUNT:KDT.OFF_TRACK_COUNT+2])[0]

        self.bpm = 120

        if self.filesize > os.path.getsize(self.path):
            sys.exit("ERROR: Indicated filesize exceeds actual filesize: %s" % self.path)

        self.buf = bytearray(self.buf[:self.filesize])

        if self.convert:
            self.midi = bytearray(self.filesize * 4)
            self.moff = 0

        if self.tracks > 0:

            self.trk_size_tbl = []

            self.trk_off_tbl = []

            self.offset = KDT.OFF_TRACK_SIZES + 2 * self.tracks

            track_size_table = self.buf[KDT.OFF_TRACK_SIZES : self.offset]

            for track_size in struct.iter_unpack("<H", track_size_table):

                self.trk_size_tbl.append(track_size[0])

                self.trk_off_tbl.append(self.offset)

                self.offset += track_size[0]

            self.set_track(0)

    def set_track(self, trknum):
        self.trknum        = trknum
        self.trk_size      = self.trk_size_tbl[trknum]
        self.trk_off_start = self.trk_off_tbl[trknum]
        self.trk_off_end   = self.trk_off_start + self.trk_size
        self.offset        = self.trk_off_start
        self.time          = 0
        self.running       = 0
        self.channel       = 0

    def read_cmd(self):
        cmd = self.buf[self.offset] & 0x7F

        param = None

        if cmd == 0x4A:
            self.running = 0
            self.offset += 1
        elif cmd == 0x4B:
            self.running = 1
            self.offset += 1
        else:
            param = self.buf[self.offset+1] & 0x7F
            self.running = self.buf[self.offset+1] & 0x80
            self.offset += 2

        if self.log:
            print("%-11s" % ("0x%02X" % cmd), end="")
            print("%-24s" % (("0x%02X" % param) if param is not None else ""), end="")

        if cmd == 0x01:
            if self.log: print("Modulation")
            if self.convert:
                self.midi[self.moff+0] = 0xB0 | self.channel
                self.midi[self.moff+1] = 0x01
                self.midi[self.moff+2] = param
                self.moff += 3

        elif cmd == 0x06:
            if self.log:
                # see PlayStation Technical Reference CD:
                # \DEVREFS\SOUNDxx.PDF
                if self.nrpn1 <= 16: # tone number
                    if self.nrpn2 == 0: # attr number
                        print("NRPN - Set Priority")
                    elif self.nrpn2 == 1:
                        print("NRPN - Set Mode")
                    elif self.nrpn2 == 2:
                        print("NRPN - Set Limit Low")
                    elif self.nrpn2 == 3:
                        print("NRPN - Set Limit High")
                    elif self.nrpn2 == 4:
                        print("NRPN - Set ADSR (AR-L)")
                    elif self.nrpn2 == 5:
                        print("NRPN - Set ADSR (AR-E)")
                    elif self.nrpn2 == 6:
                        print("NRPN - Set ADSR (DR)")
                    elif self.nrpn2 == 7:
                        print("NRPN - Set ADSR (SL)")
                    elif self.nrpn2 == 8:
                        print("NRPN - Set ADSR (SR-L)")
                    elif self.nrpn2 == 9:
                        print("NRPN - Set ADSR (SR-E)")
                    elif self.nrpn2 == 10:
                        print("NRPN - Set ADSR (RR-L)")
                    elif self.nrpn2 == 11:
                        print("NRPN - Set ADSR (RR-E)")
                    elif self.nrpn2 == 12:
                        print("NRPN - Set ADSR (SR +/-)")
                    elif self.nrpn2 == 13:
                        print("NRPN - Set Vibrate Time")
                    elif self.nrpn2 == 14:
                        print("NRPN - Set Portamento Depth")
                    elif self.nrpn2 == 15:
                        print("NRPN - Set Reverb Type")
                    elif self.nrpn2 == 16:
                        print("NRPN - Set Reverb Depth")
                    elif self.nrpn2 == 17:
                        print("NRPN - Set Echo Feedback")
                    elif self.nrpn2 == 18:
                        print("NRPN - Set Echo Delay Time")
                    elif self.nrpn2 == 19:
                        print("NRPN - Set Delay Delay Time")
                    elif self.nrpn2 == 21:
                        print("NRPN - Set Vibrate Depth")
                    elif self.nrpn2 == 22:
                        print("NRPN - Set Portamento Time")
                    else:
                        print("NRPN - Set Unknown Attribute")
                elif self.nrpn1 == 20:
                    print("NRPN - Set Loop Count")
                elif self.nrpn1 == 40:
                    print("NRPN - Set Mark Callback Value")
                else:
                    print("NRPN Data Entry")
            if self.convert:
                self.midi[self.moff+0] = 0xB0 | self.channel
                self.midi[self.moff+1] = 0x06
                self.midi[self.moff+2] = param
                self.moff += 3

        elif cmd == 0x07:
            if self.log: print("Set Volume (Channel)")
            if self.convert:
                self.midi[self.moff+0] = 0xB0 | self.channel
                self.midi[self.moff+1] = 0x07
                self.midi[self.moff+2] = param
                self.moff += 3

        elif cmd == 0x0A:
            if self.log: print("Set Panning")
            if self.convert:
                self.midi[self.moff+0] = 0xB0 | self.channel
                self.midi[self.moff+1] = 0x0A
                self.midi[self.moff+2] = param
                self.moff += 3

        elif cmd == 0x0B:
            if self.log: print("Set Volume (Expression)")
            if self.convert:
                self.midi[self.moff+0] = 0xB0 | self.channel
                self.midi[self.moff+1] = 0x0B
                self.midi[self.moff+2] = param
                self.moff += 3

        elif cmd == 0x0F:
            if self.log: print("Stereo Widening (?)")
            if self.convert:
                self.midi[self.moff:self.moff+22] = b"\xFF\x01\x13Stereo Widening (?)"
                self.moff += 22

        elif cmd == 0x40:
            if self.log: print("Damper/Sustain Pedal")
            if self.convert:
                self.midi[self.moff+0] = 0xB0 | self.channel
                self.midi[self.moff+1] = 0x40
                self.midi[self.moff+2] = param
                self.moff += 3

        elif cmd == 0x46:
            self.channel = param & 0x0F
            if self.log: print("Set Channel")
            if self.convert:
                # the tenth channel is the "drum channel" and could result in a
                # quiet track (both in Awave and fb2k)
                if self.channel >= 9:
                    self.channel = (self.channel+1) & 0x0F
                self.midi[self.moff:self.moff+14] = b"\xFF\x01\x0BSet Channel"
                self.moff += 14

        elif cmd == 0x47:
            self.bpm = min(10+param*2, 255)
            if self.log: print("Set Tempo (10-255 BPM, divisible by two)")
            if self.convert:
                # microseconds per quarter-note (beat) = microseconds per minute / beats per minute
                mpqn = 60000000 // self.bpm
                self.midi[self.moff+0] = 0xFF
                self.midi[self.moff+1] = 0x51
                self.midi[self.moff+2] = 0x03
                self.midi[self.moff+3] = (mpqn >> 16) & 0xFF
                self.midi[self.moff+4] = (mpqn >>  8) & 0xFF
                self.midi[self.moff+5] = (mpqn >>  0) & 0xFF
                self.moff += 6

        elif cmd == 0x48:
            if self.log: print("Pitch Bend")
            if self.convert:
                self.midi[self.moff+0] = 0xE0 | self.channel
                self.midi[self.moff+1] = 0 # LSB (cents)
                self.midi[self.moff+2] = param # MSB (semitones)
                self.moff += 3

        elif cmd == 0x49:
            if self.log: print("Set Instrument")
            if self.convert:
                self.midi[self.moff+0] = 0xC0 | self.channel
                self.midi[self.moff+1] = param
                self.moff += 2

        elif cmd == 0x4A:
            if self.log: print("Note Off Last Note (Reset Running Status)")
            if self.convert:
                self.midi[self.moff+0] = 0x80 | self.channel
                self.midi[self.moff+1] = self.note
                self.midi[self.moff+2] = 0
                self.moff += 3

        elif cmd == 0x4B:
            if self.log: print("Note Off Last Note (Sustain Running Status)")
            if self.convert:
                self.midi[self.moff+0] = 0x80 | self.channel
                self.midi[self.moff+1] = self.note
                self.midi[self.moff+2] = 0
                self.moff += 3

        elif cmd == 0x4C:
            self.bpm = param & 0x7F
            if self.log: print("Set Tempo (0-127 BPM)")
            if self.convert:
                mpqn = 60000000 // self.bpm
                self.midi[self.moff+0] = 0xFF
                self.midi[self.moff+1] = 0x51
                self.midi[self.moff+2] = 0x03
                self.midi[self.moff+3] = (mpqn >> 16) & 0xFF
                self.midi[self.moff+4] = (mpqn >>  8) & 0xFF
                self.midi[self.moff+5] = (mpqn >>  0) & 0xFF
                self.moff += 6

        elif cmd == 0x4D:
            self.bpm = param | 0x80
            if self.log: print("Set Tempo (128-255 BPM)")
            if self.convert:
                mpqn = 60000000 // self.bpm
                self.midi[self.moff+0] = 0xFF
                self.midi[self.moff+1] = 0x51
                self.midi[self.moff+2] = 0x03
                self.midi[self.moff+3] = (mpqn >> 16) & 0xFF
                self.midi[self.moff+4] = (mpqn >>  8) & 0xFF
                self.midi[self.moff+5] = (mpqn >>  0) & 0xFF
                self.moff += 6

        elif cmd == 0x5B:
            if self.log: print("Set Reverb Depth")
            if self.convert:
                self.midi[self.moff+0] = 0xB0 | self.channel
                self.midi[self.moff+1] = 0x5B
                self.midi[self.moff+2] = param
                self.moff += 3

        elif cmd == 0x62:
            if self.log: print("NRPN (LSB)")
            if self.convert:
                self.midi[self.moff:self.moff+13] = b"\xFF\x01\x0ANRPN (LSB)"
                self.moff += 13
            self.nrpn2 = param

        elif cmd == 0x63:
            if param == 20:
                if self.log: print("NRPN - Set Loop Start")
                if self.convert:
                    self.midi[self.moff:self.moff+13] = b"\xFF\x01\x0ALoop Start"
                    self.moff += 13
            elif param == 30:
                if self.log: print("NRPN - Set Loop End")
                if self.convert:
                    self.midi[self.moff:self.moff+11] = b"\xFF\x01\x08Loop End"
                    self.moff += 11
            elif param == 40:
                if self.log: print("NRPN - Set Mark")
                if self.convert:
                    self.midi[self.moff:self.moff+7] = b"\xFF\x01\x04Mark"
                    self.moff += 7
            else:
                if self.log: print("NRPN (MSB)")
                if self.convert:
                    self.midi[self.moff:self.moff+13] = b"\xFF\x01\x0ANRPN (MSB)"
                    self.moff += 13
            self.nrpn1 = param

        elif cmd == 0x76:
            if self.log: print("Seq Beat")
            if self.convert:
                self.midi[self.moff:self.moff+11] = b"\xFF\x01\x08Seq Beat"
                self.moff += 11

        elif cmd == 0x7F:
            if self.log: print("End of Track")
            if self.convert:
                self.midi[self.moff+0] = 0xFF
                self.midi[self.moff+1] = 0x2F
                self.midi[self.moff+2] = 0x00
                self.moff += 3

        else:
            if self.log: print("Unknown")
            if self.convert:
                self.midi[self.moff:self.moff+4] = b"\xFF\x01\x01\x3F"
                self.moff += 4

    def read_note(self):
        self.note = self.buf[self.offset] & 0x7F
        self.velocity = self.buf[self.offset+1] & 0x7F
        self.running = self.buf[self.offset+1] & 0x80
        self.offset += 2
        if self.log: print(
            "%-11s%s" % (
                "0x%02X" % self.note,
                "0x%02X" % self.velocity
            )
        )
        if self.convert:
            self.midi[self.moff+0] = (0x90 if self.velocity else 0x80) | self.channel
            self.midi[self.moff+1] = self.note
            self.midi[self.moff+2] = self.velocity
            self.moff += 3

    def read_delta_time(self):
        if self.convert:
            self.midi[self.moff] = self.buf[self.offset]
            self.moff += 1
        ticks = self.buf[self.offset] & 0x7F
        more = self.buf[self.offset] & 0x80
        self.offset += 1
        while more:
            if self.convert:
                self.midi[self.moff] = self.buf[self.offset]
                self.moff += 1
            ticks <<= 7
            ticks |= self.buf[self.offset] & 0x7F
            more = self.buf[self.offset] & 0x80
            self.offset += 1
        if self.log: print("%d" % ticks)
        self.time += ticks
        self.running = 1

    def read_seq(self):
        if self.log:
            print("%-10s" % ("0x%04X" % (self.offset - self.trk_off_start)), end="")

            mm, ss = divmod(self.time * 60 / self.tickdiv / self.bpm, 60)

            print("%-22s" % ("%d (%02d:%07.4f)" % (self.time, int(mm), ss)), end="")

        if not self.running:
            if self.log: print("%-11s" % "Time", end="")
            self.read_delta_time()
        else:
            if self.buf[self.offset] & 0x80:
                if self.log: print("%-11s" % "Command", end="")
                self.read_cmd()
            else:
                if self.log: print("%-11s" % "Key", end="")
                self.read_note()

            # instead of having delta-times of 0 between events, KDT1 uses the
            # MSB in the command parameter / note velocity to save some bytes
            if self.convert:
                if self.running:
                    if self.offset < self.trk_off_end:
                        self.midi[self.moff] = 0
                        self.moff += 1

def kdt2midi(path):
    kdt = KDT(path, log=False, convert=True)

    # 0x00: file format identifier
    kdt.midi[kdt.moff:kdt.moff+4] = b"MThd"
    kdt.moff += 4

    # 0x04: size of header: 6
    kdt.midi[kdt.moff:kdt.moff+4] = b"\x00\x00\x00\x06"
    kdt.moff += 4

    # 0x08: MIDI type: 1
    kdt.midi[kdt.moff:kdt.moff+2] = b"\x00\x01"
    kdt.moff += 2

    # 0x0A: number of tracks
    kdt.midi[kdt.moff:kdt.moff+2] = struct.pack(">H", kdt.tracks)
    kdt.moff += 2

    # 0x0C: pulses per quarter-note
    kdt.midi[kdt.moff:kdt.moff+2] = struct.pack(">H", kdt.tickdiv)
    kdt.moff += 2

    for trknum in range(kdt.tracks):
        kdt.set_track(trknum)

        mtrkoff = kdt.moff

        # 0x00: track chunk identifier
        kdt.midi[kdt.moff:kdt.moff+4] = b"MTrk"
        kdt.moff += 4

        # 0x04: track size (temporary)
        kdt.midi[kdt.moff:kdt.moff+4] = b"\x00\x00\x00\x00"
        kdt.moff += 4

        # 0x08: delta time | meta event | meta type: track name | length of track name
        kdt.midi[kdt.moff:kdt.moff+4] = b"\x00\xFF\x03\x08"
        kdt.moff += 4

        # 0x0C: track name
        kdt.midi[kdt.moff:kdt.moff+8] = bytes("Track %02d" % trknum, encoding="ascii")
        kdt.moff += 8

        while kdt.offset < kdt.trk_off_end:
            kdt.read_seq()

        kdt.midi[mtrkoff+4:mtrkoff+8] = struct.pack(">I", kdt.moff-mtrkoff-8)

    with open(os.path.splitext(kdt.path)[0] + ".MID", "wb") as midi:
        midi.write(kdt.midi[:kdt.moff])

def dump_events(path):
    kdt = KDT(path, log=True, convert=False)

    for trknum in range(kdt.tracks):

        kdt.set_track(trknum)

        print("TRACK %02d (0x%04X)" % (trknum, kdt.offset))
        print("========================================================================================================================")
        print("Offset    Time                  Event      Value      Parameter / Velocity    Description")
        print()

        while kdt.offset < kdt.trk_off_end:

            kdt.read_seq()

        print("\n" * 5)

def main(argc=len(sys.argv), argv=sys.argv):

    if argc != 3:
        script_name = os.path.basename(argv[0])
        print("Usage:")
        print("  Convert to MIDI: %s -c <path>" % script_name)
        print("  Dump event data: %s -l <path>" % script_name)
        return 1

    path = os.path.realpath(argv[-1])

    if not os.path.isfile(path):
        print("ERROR: Invalid path")
        return 1

    if argv[1] == '-c':
        kdt2midi(path)
    elif argv[1] == '-l':
        dump_events(path)
    else:
        print("ERROR: Invalid command line")
        return 1

    return 0

if __name__=="__main__":
    main()
