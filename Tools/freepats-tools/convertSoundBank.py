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
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')

import re, textwrap
from sfz import SFZ
from sf2 import SF2

inputFormats = ['sfz']
outputFormats = ['sfz', 'sf2']

if len(sys.argv) < 3:
	print("Usage:", sys.argv[0], "INPUT OUTPUT\n")
	print(textwrap.dedent("""
		Process INPUT sound bank and writes an OUTPUT file, which can be in different
		format. It tries to guess formats from file names. Supported formats in this
		version:
	""").strip())
	print("")
	print("    Input:", ", ".join(inputFormats).upper())
	print("    Output:", ", ".join(outputFormats).upper())
	print("")
	print(textwrap.dedent("""
		This program supports a limited subset of the SFZ format, extended with
		annotations which enable better control of the generated output files.
	""").strip())
	sys.exit(1)

inputFile = sys.argv[1]
inputFormat = None
outputFile = sys.argv[2]
outputFormat = None
soundBank = None

match = re.search('\.([a-z0-9]+)$', inputFile.lower())
if match:
	inputFormat = match.group(1)
else:
	logging.error("Can not guess format from file name: {}".format(inputFile))
	sys.exit(1)

if not inputFormat in inputFormats:
	logging.error("Unknown or unsupported input format: {}".format(inputFormat))
	sys.exit(1)

match = re.search('\.([a-z0-9]+)$', outputFile.lower())
if match:
	outputFormat = match.group(1)
else:
	logging.error("Can not guess format from file name: {}".format(outputFile))
	sys.exit(1)

if not outputFormat in outputFormats:
	logging.error("Unknown or unsupported output format: {}".format(outputFormat))
	sys.exit(1)

print("Reading and processing input file...")
if inputFormat == 'sfz':
	sfz = SFZ()
	if not sfz.importSFZ(sys.argv[1]):
		sys.exit(1)
	soundBank = sfz.soundBank

print("Writing output file...")
if outputFormat == 'sfz':
	sfz = SFZ()
	sfz.soundBank = soundBank
	if not sfz.exportSFZ(outputFile):
		sys.exit(1)
elif outputFormat == 'sf2':
	sf2 = SF2()
	if not sf2.exportSF2(soundBank, outputFile):
		sys.exit(1)

print("Done")
