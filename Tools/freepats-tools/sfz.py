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

import logging, re, os.path, sys
import dateutil.parser


class SFZParseError(Exception):
	pass


class SFZ:

	noteValue = {'C': 0, 'D': 2, 'E': 4, 'F': 5, 'G': 7, 'A': 9, 'B': 11}


	def importSFZ(self, fileName):
		self.soundBank = {'instruments': []}
		self.instrument = {'groups': []}
		self.group = {'regions': []}
		self.region = {}
		self.insideInstrument = False
		self.insideGroup = False
		self.insideRegion = False
		try:
			inFile = open(fileName, 'r')
		except:
			logging.error("Can not open file: {}".format(fileName))
			return False
		path = os.path.dirname(fileName)
		if len(path) > 0:
			self.soundBank['Path'] = path

		lineNumber = 0
		for line in inFile:
			lineNumber += 1
			try:
				self.processLine(line)
			except SFZParseError:
				logging.error("Error on line {} of file {}".format(lineNumber, fileName))
				inFile.close()
				return False

		self.commitRegion()
		self.commitGroup()
		self.commitInstrument()
		inFile.close()
		return True


	def exportSFZ(self, fileName = None):
		if fileName:
			outFile = open(fileName, 'w')
		else:
			outFile = sys.stdout
		for hint in ('Name', 'Date', 'URL'):
			if hint in self.soundBank.keys():
				outFile.write('//+ {}: {}\n'.format(hint, self.soundBank[hint]))

		for instrument in self.soundBank['instruments']:
			if len(self.soundBank['instruments']) > 1 or len(instrument) > 1:
				outFile.write('\n<global>\n')
				for instKey in sorted(instrument.keys()):
					if instKey[0].isupper():
						outFile.write(' //+ {}: {}\n'.format(instKey, instrument[instKey]))
					elif instKey != 'groups':
						outFile.write(' {}={}\n'.format(instKey, instrument[instKey]))
			for group in instrument['groups']:
				outFile.write('\n<group>\n')
				for groupKey in sorted(group.keys()):
					if groupKey != 'regions':
						outFile.write(' {}={}\n'.format(groupKey, group[groupKey]))
				for region in group['regions']:
					outFile.write('<region>\n')

					# if hikey, lokey and pitch_keycenter are set to the same value,
					# write a single key opcode
					hikey = 127
					lokey = 0
					pitch = 60
					if 'hikey' in region.keys():
						hikey = region['hikey']
					if 'lokey' in region.keys():
						lokey = region['lokey']
					if 'pitch_keycenter' in region.keys():
						pitch = region['pitch_keycenter']
					if hikey == lokey and hikey == pitch:
						outFile.write(' key={}\n'.format(region['hikey']))
					else:
						if lokey != 0:
							outFile.write(' lokey={}\n'.format(lokey))
						if hikey != 127:
							outFile.write(' hikey={}\n'.format(hikey))
						if 'pitch_keycenter' in region.keys():
							outFile.write(' pitch_keycenter={}\n'.format(pitch))

					for regionKey in sorted(region.keys()):
						if regionKey == 'hikey' \
						or regionKey == 'lokey' \
						or regionKey == 'pitch_keycenter':
							continue
						outFile.write(' {}={}\n'.format(regionKey, region[regionKey]))
		outFile.close()
		return True


	def processLine(self, line):
		match = re.search('//\+ ([a-zA-Z0-9_&.+-]+): +(\S.*)$', line)
		if match:
			value = match.group(2)
			value = value.rstrip()
			return self.processHint(match.group(1), value)

		line = line.partition('//')[0] # Erase comments
		line = line.rstrip()

		while True:
			line = line.lstrip()
			if len(line) == 0:
				return True

			# Header
			if line[0] == '<':
				end = line.find('>')
				if end == -1:
					raise SFZParseError
				header = line[1:end]
				if len(header) < 1:
					raise SFZParseError
				self.processHeader(header)
				line = line[end+1:]
				continue

			# Opcode
			end = line.find('=')
			if end == -1:
				raise SFZParseError
			opcode = line[:end]
			if len(opcode) < 1:
				raise SFZParseError
			line = line[end+1:]
			if len(line) == 0:
				raise SFZParseError

			# Find next opcode or header
			match = re.search('[=<]', line)
			if not match:
				return self.processOpcode(opcode, line)

			if line[match.start()] == '=':
				nextOpcode = re.search('\s[a-zA-Z0-9_]+=', line)
				if not nextOpcode:
					raise SFZParseError
				value = line[:nextOpcode.start()].rstrip()
				line = line[nextOpcode.start():]
			else:
				value = line[:match.start()].rstrip()
				line = line[match.start():]

			self.processOpcode(opcode, value)

		return False


	def processHeader(self, header):
		if header == 'global':
			self.commitRegion()
			self.commitGroup()
			self.commitInstrument()
			self.insideInstrument = True
			self.insideGroup = False
			self.insideRegion = False
		elif header == 'group':
			self.commitRegion()
			self.commitGroup()
			self.insideInstrument = True
			self.insideGroup = True
			self.insideRegion = False
		elif header == 'region':
			self.commitRegion()
			self.insideInstrument = True
			self.insideRegion = True
		else:
			raise SFZParseError


	def getOpcode(self, opcode, instrument = None, group = None, region = None, default = None):
		if region and opcode in region.keys():
			return region[opcode]
		elif group and opcode in group.keys():
			return group[opcode]
		elif instrument and opcode in instrument.keys():
			return instrument[opcode]
		return default


	def commitInstrument(self):
		if len(self.instrument['groups']) > 0:
			if self.getOpcode('loop_start', self.instrument) != None \
			and self.getOpcode('loop_end', self.instrument) != None \
			and not self.getOpcode('loop_mode', self.instrument):
				self.instrument['loop_mode'] = 'loop_continuous'
			self.soundBank['instruments'].append(self.instrument)
		self.instrument = {'groups': []}


	def commitGroup(self):
		if len(self.group['regions']) > 0:
			if self.getOpcode('loop_start', self.instrument, self.group) != None \
			and self.getOpcode('loop_end', self.instrument, self.group) != None \
			and not self.getOpcode('loop_mode', self.instrument, self.group):
				self.group['loop_mode'] = 'loop_continuous'
			self.instrument['groups'].append(self.group)
		self.group = {'regions': []}


	def commitRegion(self):
		if len(self.region) > 0:
			if self.getOpcode('loop_start', self.instrument, self.group, self.region) != None \
			and self.getOpcode('loop_end', self.instrument, self.group, self.region) != None \
			and not self.getOpcode('loop_mode', self.instrument, self.group, self.region):
				self.region['loop_mode'] = 'loop_continuous'
			self.group['regions'].append(self.region)
		self.region = {}


	def processOpcode(self, opcode, value):
		if opcode == 'sample':
			value = os.path.normpath(value.replace('\\', '/'))
		elif opcode == 'lokey' \
		or opcode == 'hikey' \
		or opcode == 'pitch_keycenter':
			value = self.convertNote(value)
		elif opcode == 'key':
			value = self.convertNote(value)
			self.addOpcode('hikey', value)
			self.addOpcode('lokey', value)
			self.addOpcode('pitch_keycenter', value)
			return True
		elif opcode == 'lovel' \
		or opcode == 'hivel':
			value = self.convertNumberI(value, -1, 127)
		elif opcode == 'volume':
			value = self.convertNumberF(value, -144, 6)
		elif opcode == 'tune':
			value = self.convertNumberI(value, -100, 100)
		elif opcode == 'pitch_keytrack':
			value = self.convertNumberI(value, -1200, 1200)
		elif opcode == 'ampeg_attack' \
		or opcode == 'ampeg_decay' \
		or opcode == 'ampeg_sustain' \
		or opcode == 'ampeg_hold' \
		or opcode == 'ampeg_release':
			value = self.convertNumberF(value, 0, 100)
		elif opcode == 'loop_start' \
		or opcode == 'loop_end':
			value = self.convertNumberI(value, 0, 4294967296)
		elif opcode == 'loop_mode':
			if not value in ['no_loop', 'one_shot', 'loop_continuous', 'loop_sustain']:
				logging.error("Unknown parameter for loop_mode: {}".format(value))
				raise SFZParseError
		elif opcode == 'fil_type':
			if not value in ['lpf_1p', 'hpf_1p', 'lpf_2p', 'hpf_2p', 'bpf_2p', 'brf_2p']:
				logging.error("Unknown parameter for fil_type: {}".format(value))
				raise SFZParseError
		elif opcode == 'cutoff':
			value = self.convertNumberF(value, 0, 2822400)
		elif opcode == 'resonance':
			value = self.convertNumberF(value, 0, 40)
		elif opcode == 'delay':
			value = self.convertNumberF(value, 0, 100)
		elif opcode == 'lorand':
			value = self.convertNumberF(value, 0, 1)
		elif opcode == 'hirand':
			value = self.convertNumberF(value, 0, 1)
		elif opcode == 'pan':
			value = self.convertNumberF(value, -100, 100)
		elif opcode == 'seq_length':
			value = self.convertNumberI(value, 1, 100)
		elif opcode == 'seq_position':
			value = self.convertNumberI(value, 1, 100)
		elif opcode == 'amp_veltrack':
			value = self.convertNumberF(value, -100, 100)
		else:
			logging.warning("Unknown opcode: {}".format(opcode))
			return True

		self.addOpcode(opcode, value)
		return True


	def addOpcode(self, opcode, value):
		if self.insideRegion:
			self.region[opcode] = value
		elif self.insideGroup:
			self.group[opcode] = value
		elif self.insideInstrument:
			self.instrument[opcode] = value
		else:
			self.soundBank[opcode] = value


	def processHint(self, var, value):
		if var == 'Name':
			if self.insideGroup:
				raise SFZParseError
		elif var == 'Date':
			if self.insideGroup:
				raise SFZParseError
			try:
				date = dateutil.parser.parse(value)
			except:
				logging.error("Invalid or unknown date format: {}".format(value))
				raise SFZParseError
			value = date.strftime('%Y-%m-%d')
		elif var == 'URL':
			if self.insideGroup:
				raise SFZParseError
			if not value.startswith(('http://', 'https://', 'ftp://', 'file:')):
				raise SFZParseError
		elif var == 'Instrument':
			if self.insideGroup:
				raise SFZParseError
		elif var == 'Program':
			if self.insideGroup:
				raise SFZParseError
			value = self.convertNumberI(value, 1, 128)
		elif var == 'PercussionMode':
			if self.insideGroup or not self.insideInstrument:
				raise SFZParseError
			value = self.convertBoolean(value)
		elif var == 'RandomRegion':
			if self.insideRegion or not self.insideGroup:
				raise SFZParseError
			value = self.convertBoolean(value)
		else:
			return True

		self.addOpcode(var, value)
		return True


	def convertNumberI(self, numS, minVal, maxVal):
		if not re.search('^-?[0-9]+$', numS):
			raise SFZParseError
		num = int(numS)
		if num < minVal or num > maxVal:
			raise SFZParseError
		return num


	def convertNumberF(self, numS, minVal, maxVal):
		if not re.search('^-?[0-9]*.?[0-9]+$', numS):
			raise SFZParseError
		num = float(numS)
		if num < minVal or num > maxVal:
			raise SFZParseError
		return num

	def convertBoolean(self, value):
		if value == 'Yes':
			return True
		elif value == 'No':
			return False
		raise SFZParseError

	def convertNote(self, note):
		if re.search('^[0-9]{1,3}$', note):
			noteNum = int(note)
			if noteNum >= 0 and noteNum <= 127:
				return noteNum;

		match = re.search('^([abcdefgABCDEFG])([b#]?)(-?[0-9])$', note)
		if not match:
			raise SFZParseError
		noteNum = SFZ.noteValue[match.group(1).upper()]
		if match.group(2):
			if match.group(2) == '#':
				noteNum += 1
			elif match.group(2) == 'b':
				noteNum -= 1
		octave = int(match.group(3))
		if octave < -1 or octave > 9:
			raise SFZParseError
		noteNum += (octave + 1) * 12
		if noteNum > 127:
			raise SFZParseError
		return noteNum

