#!/usr/bin/python3
#
# Copyright 2016, roberto@zenvoid.org
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

import sys, logging
logging.basicConfig(level=logging.INFO, stream=sys.stderr, format='%(levelname)s: %(message)s')

import re, os.path, time, textwrap
from sfz import SFZ

if len(sys.argv) < 2:
	print("Usage:", sys.argv[0], "[SAMPLE]...\n", file=sys.stderr)
	print(textwrap.dedent("""
		Takes audio files as input and writes to stdout a SFZ template for them.
		It tries to guess the pitch of each sample from its file name.

		Examples:
	""").strip(), file=sys.stderr)
	print("")
	print("    {}".format(sys.argv[0]), "samples/*.wav", file=sys.stderr)
	print("    {}".format(sys.argv[0]), "piano_C4.wav piano_C5.wav piano_F#4.wav", file=sys.stderr)
	sys.exit(0)

sfz = SFZ()
regions = {}

noteRegEx = re.compile('^(.+[-_])?(([abcdefgABCDEFG])([b#]?)(-?[0-9]))(v(([0-9]{1,3})|[LMHlmh]))?([-_][0-9]+)?\.wav$')
numRegEx = re.compile('^(.+[-_])?([0-9]{1,3})(v(([0-9]{1,3})|[LMHlmh]))?([-_][0-9]+)?\.wav')

for fName in sys.argv[1:]:
	match = noteRegEx.search(os.path.basename(fName))
	if match:
		noteNum = sfz.convertNote(match.group(2))
		if noteNum == None:
			logging.warning("Can't guess pitch from file name: {}".format(fName))
			continue
		regions[noteNum] = fName
		continue
	match = numRegEx.search(os.path.basename(fName))
	if match:
		noteNum = int(match.group(2))
		if noteNum < 0 or noteNum > 127:
			logging.warning("Can't guess pitch from file name: {}".format(fName))
			continue
		regions[noteNum] = fName
		continue
	logging.warning("Can't guess pitch from file name: {}".format(fName))

soundBank = {
'Name': 'Unnamed sound bank',
'Date': time.strftime("%Y-%m-%d"),
'instruments': [{
	'Instrument': 'Unnamed instrument',
	'ampeg_release': '0.5',
	'groups': [{
		'loop_mode': 'no_loop',
	    'regions': []
    	}]
	}]
}

prevRegion = None
for noteNum in sorted(regions.keys()):
	region = {}
	region['sample'] = regions[noteNum]
	region['pitch_keycenter'] = noteNum
	if prevRegion:
		gap = noteNum - prevRegion['pitch_keycenter'] - 1
		leftGap = gap // 2
		rightGap = gap - leftGap
		prevRegion['hikey'] = prevRegion['pitch_keycenter'] + leftGap
		region['lokey'] = noteNum - rightGap
	soundBank['instruments'][0]['groups'][0]['regions'].append(region)
	prevRegion = soundBank['instruments'][0]['groups'][0]['regions'][-1]

sfz.soundBank = soundBank
sfz.exportSFZ()
