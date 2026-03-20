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

# While the implementation of this module is new, the idea of building a SF2
# file from python lists comes from the pysf utility, a public domain program
# to convert from XML descriptions to SoundFont files:
# https://github.com/freepats/tools

import struct, logging, os, math, sys
import dateutil.parser
import soundfile


class SF2ExportError(Exception):
	pass


class SF2:

	sfGenId = {
		'initialFilterFc': 8,
		'initialFilterQ': 9,
		'pan': 17,
		'delayVolEnv': 33,
		'attackVolEnv': 34,
		'holdVolEnv': 35,
		'decayVolEnv': 36,
		'sustainVolEnv': 37,
		'releaseVolEnv': 38,
		'instrument': 41,
		'keyRange': 43,
		'velRange': 44,
		'velocity': 47,
		'initialAttenuation': 48,
		'fineTune': 52,
		'sampleID': 53,
		'sampleModes': 54,
		'scaleTuning': 56,
		'overridingRootKey': 58
	}

	sfGenType = {
		'delayVolEnv': 'h',
		'attackVolEnv': 'h',
		'decayVolEnv': 'h',
		'sustainVolEnv': 'h',
		'holdVolEnv': 'h',
		'releaseVolEnv': 'h',
		'initialFilterFc': 'h',
		'initialFilterQ': 'h',
		'initialAttenuation': 'h',
		'fineTune': 'h',
		'scaleTuning': 'h'
	}

	def exportSF2(self, soundBank, fileName):
		self.soundBank = soundBank
		self.nextProgram = 0
		try:
			self.outFile = open(fileName, 'wb')
		except:
			logging.error("Can not create file {}".format(fileName))
			return False

		try:
			sf2 = [[[b'RIFF', b'sfbk'], [
				self.sfInfo(),
				self.sfSdta(),
				self.sfPdta()
			]]]

			self.exportChunks(sf2)
		except SF2ExportError:
			self.outFile.close()
			os.unlink(fileName)
			logging.error("Failed to export SF2 to file {}".format(fileName))
			return False
		except:
			self.outFile.close()
			os.unlink(fileName)
			logging.error("Failed to export SF2 to file {}".format(fileName))
			raise

		self.outFile.close()
		self.outFile = None
		self.sampleList = {}
		self.shdrData = bytearray()
		return True


	def exportChunks(self, chunks):
		for chunk in chunks:
			(key, data) = chunk
			form = None
			if type(key) == list:
				(key, form) = key
			chunkPos = self.outFile.tell()
			self.outFile.write(struct.pack('<4sI', key, 0))
			dataStart = self.outFile.tell()
			if form:
				self.outFile.write(struct.pack('<4s', form))

			if type(data) == list:
				self.exportChunks(data)
			else:
				self.outFile.write(data)

			dataEnd = self.outFile.tell()
			dataSize = dataEnd - dataStart
			self.outFile.seek(chunkPos + 4)
			self.outFile.write(struct.pack('<I', dataSize))
			self.outFile.seek(dataEnd)


	def getOpcode(self, opcode, instrument = None, group = None, region = None, default = None):
		if region and opcode in region.keys():
			return region[opcode]
		elif group and opcode in group.keys():
			return group[opcode]
		elif instrument and opcode in instrument.keys():
			return instrument[opcode]
		return default


	def sfPackString(self, string, maxLength = 256):
		if string == None:
			return None

		length = len(string) + 1
		if length % 2 > 0:
			length += 1

		if length > maxLength:
			newString = string[0:maxLength-1]
			logging.warning("Truncating string: {}".format(string))
			string = newString
			length = maxLength

		return struct.pack('{}s'.format(length), string.encode('ascii'))


	def percentToCentibels(self, percent):
		percent = float(percent)
		return int(round(200 * math.log10(100 / percent)))


	def freqToAbsoluteCents(self, freq):
		freq = float(freq)
		if freq == 0:
			return 1500
		value = int(round(1200 * math.log2(freq/440) + 6900))
		if value < 1500:
			value = 1500
		if value > 13500:
			value = 13500
		return value


	def genTime(self, seconds):
		if seconds == 0:
			return -32768
		value = int(round(1200 * math.log2(seconds)))
		if value < -32768:
			return -32768
		if value > 32767:
			return 32767
		return value


	def sfInfo(self):
		sfMajor = 2
		sfMinor = 1
		name = 'Sound Bank'
		comments = ''

		chunk = [[b'LIST', b'INFO'], [
			[b'ifil', struct.pack('<2H', sfMajor, sfMinor)],
			[b'isng', self.sfPackString('EMU8000')]
		]]
		if 'Name' in self.soundBank.keys():
			name = self.soundBank['Name']
		chunk[1].append([b'INAM', self.sfPackString(name)])
		if 'Date' in self.soundBank.keys():
			date = dateutil.parser.parse(self.soundBank['Date'])
			chunk[1].append([b'ICRD', self.sfPackString(date.strftime('%b %d, %Y'))])
		if 'Author' in self.soundBank.keys():
			chunk[1].append([b'IENG', self.sfPackString(self.soundBank['Author'])])
		if 'URL' in self.soundBank.keys():
			chunk[1].append([b'ICMT', self.sfPackString(self.soundBank['URL'])])
		return chunk


	def sfSdta(self):
		sampleIndex = 0
		self.sampleList = {}
		self.shdrData = bytearray()
		smplData = bytearray()
		for instrument in self.soundBank['instruments']:
			for group in instrument['groups']:
				for region in group['regions']:
					sample = self.getOpcode('sample', instrument, group, region)
					if not sample or sample in self.sampleList.keys():
						continue

					samplePath = sample
					if not os.path.isabs(samplePath) and 'Path' in self.soundBank.keys():
						samplePath = os.path.join(self.soundBank['Path'], sample)
					try:
						data, rate = soundfile.read(file=samplePath, dtype='int16', always_2d=True)
					except:
						logging.error("Can not read input audio file {}".format(samplePath))
						raise SF2ExportError
					channels = len(data[0])
					if channels < 1:
						logging.error("Can not read data from audio file {}".format(samplePath))
						raise SF2ExportError
					if channels > 2:
						logging.error("Audio file contains more than 2 channels: {}".format(samplePath))
						raise SF2ExportError

					pitch = self.getOpcode('pitch_keycenter', instrument, group, region, 60)
					self.sampleList[sample] = [channels, sampleIndex, pitch]
					for ch in range(0, channels):
						start = len(smplData) // 2
						for n in data:
							smplData += struct.pack('<h', n[ch])
						end = len(smplData) // 2
						smplData += bytes(46 * 2)

						sampleType = 1 # mono sample
						if channels == 2:
							if ch == 0:
								sampleType = 4 # left sample
							else:
								sampleType = 2 # right sample

						loopMode = self.getOpcode('loop_mode', instrument, group, region, 'no_loop')
						loopStartDefault = 0
						loopEndDefault = end - start
						if loopMode == 'no_loop':
							loopStartDefault += 8
							loopEndDefault -= 8
						loopStart = start + self.getOpcode('loop_start', instrument, group, region, loopStartDefault)
						loopEnd = start + self.getOpcode('loop_end', instrument, group, region, loopEndDefault)
						name, ext = os.path.splitext(os.path.basename(sample))
						sampleLink = 0
						if channels == 2:
							if ch == 0:
								name += '_L'
								sampleLink = sampleIndex + 1
							else:
								name += '_R'
								sampleLink = sampleIndex - 1
						self.shdrData += struct.pack('<19sBIIIIIBbHH',
							name.encode('ascii'), 0, start, end, loopStart, loopEnd, rate, pitch, 0,
							sampleLink, sampleType)
						sampleIndex += 1

		return [[b'LIST', b'sdta'], [
			[b'smpl', smplData]
		]]


	def createGenList(self, instrument = None, group = None, region = None):
		genList = {}
		genOpcodes = {
			'delayVolEnv': 'delay',
			'attackVolEnv': 'ampeg_attack',
			'decayVolEnv': 'ampeg_decay',
			'sustainVolEnv': 'ampeg_sustain',
			'holdVolEnv': 'ampeg_hold',
			'releaseVolEnv': 'ampeg_release',
			'initialFilterFc': 'cutoff',
			'initialFilterQ': 'resonance',
			'initialAttenuation': 'volume',
			'fineTune': 'tune',
			'scaleTuning': 'pitch_keytrack'
		}
		for gen in genOpcodes.keys():
			value = self.getOpcode(genOpcodes[gen], instrument, group, region)
			if value == None:
				continue
			if gen in ['delayVolEnv', 'attackVolEnv', 'decayVolEnv', 'holdVolEnv', 'releaseVolEnv']:
				genList[gen] = self.genTime(value)
			elif gen == 'sustainVolEnv':
				value = float(value)
				if value == 0:
					genList[gen] = 1000
				else:
					genList[gen] = self.percentToCentibels(value)
					if genList[gen] > 1000:
						genList[gen] = 1000
			elif gen == 'initialFilterFc':
				fil_type = self.getOpcode('fil_type', instrument, group, region)
				if fil_type == None or fil_type == 'lpf_2p':
					genList[gen] = self.freqToAbsoluteCents(value)
				else:
					logging.error("SF2 format does not support filter type {}".format(fil_type))
					raise SF2ExportError
			elif gen == 'initialFilterQ':
				genList[gen] = int(float(value) * 10)
			elif gen == 'initialAttenuation':
				value = float(value)
				if value > 0:
					logging.warning("SF2 format does not support amplification (positive volume value)")
				genList[gen] = int(-value * 10)
			elif gen == 'fineTune':
				genList[gen] = int(value)
			elif gen == 'scaleTuning':
				genList[gen] = int(value)

		loopMode = self.getOpcode('loop_mode', instrument, group, region, 'no_loop')
		if loopMode == 'one_shot':
			# Simulate one_shot mode using a large value for releaseVolEnv
			genList['releaseVolEnv'] = self.genTime(100)

		return genList


	def getKeyRange(self, instrument):
		lokeyMin = 128
		hikeyMax = -1
		for group in instrument['groups']:
			for region in group['regions']:
				lokey = self.getOpcode('lokey', instrument, group, region, 0)
				if lokey < lokeyMin:
					lokeyMin = lokey
				hikey = self.getOpcode('hikey', instrument, group, region, 127)
				if hikey > hikeyMax:
					hikeyMax = hikey
		if lokeyMin == 128:
			lokeyMin = 0
		if hikeyMax == -1:
			hikeyMax = 127
		return lokeyMin, hikeyMax


	def sfPdta(self):
		instNum = 0
		pbagNdx = 0
		pgenNdx = 0
		ibagNdx = 0
		igenNdx = 0
		phdrData = bytearray()
		pbagData = bytearray()
		pgenData = bytearray()
		instData = bytearray()
		ibagData = bytearray()
		igenData = bytearray()

		if 'Instrument' in self.soundBank.keys():
			# Create a main preset which includes all instruments

			instrumentName = self.soundBank['Instrument']
			program = self.nextProgram
			if 'Program' in self.soundBank.keys():
				program = self.soundBank['Program'] - 1
			else:
				self.nextProgram += 1
			phdrData += struct.pack('<19sBHHHIII', instrumentName.encode('ascii'), 0, program, 0, pbagNdx, 0, 0, 0)

			for instrument in self.soundBank['instruments']:
				pbagData += struct.pack('<HH', pgenNdx, 0)
				pbagNdx += 1

				# Instrument options (main preset)
				# --------------------------------

				# keyRange (if exists, it must be the first)
				keyMin, keyMax = self.getKeyRange(instrument)
				if keyMin > 0 or keyMax < 127:
					pgenData += struct.pack('<HBB', SF2.sfGenId['keyRange'], keyMin, keyMax)
					pgenNdx += 1

				# velRange (if exists, it must be preceded only by keyRange)
				lovel = 0
				hivel = 127
				if 'lovel' in instrument.keys():
					lovel = instrument['lovel']
				if 'hivel' in instrument.keys():
					hivel = instrument['hivel']
				if lovel > 0 or hivel < 127:
					pgenData += struct.pack('<HBB', SF2.sfGenId['velRange'], lovel, hivel)
					pgenNdx += 1

				# instrument (it must be the last)
				pgenData += struct.pack('<HH', SF2.sfGenId['instrument'], instNum)
				pgenNdx += 1
				instNum += 1

		instNum = 0
		for instrument in self.soundBank['instruments']:
			instrumentName = 'Instrument'
			if 'Instrument' in instrument.keys():
				instrumentName = instrument['Instrument']
			elif 'Instrument' in self.soundBank.keys():
				instrumentName = self.soundBank['Instrument']
			elif 'Name' in self.soundBank.keys():
				instrumentName = self.soundBank['Name']
			createPreset = True
			program = self.nextProgram
			if 'Program' in instrument.keys():
				program = instrument['Program'] - 1
			elif 'Instrument' in self.soundBank.keys():
				createPreset = False
			else:
				self.nextProgram += 1

			if createPreset:
				bank = 0
				if self.getOpcode('PercussionMode', instrument, default = False):
					bank = 128
				phdrData += struct.pack('<19sBHHHIII', instrumentName.encode('ascii'), 0,
					program, bank, pbagNdx, 0, 0, 0)
				pbagData += struct.pack('<HH', pgenNdx, 0)
				pbagNdx += 1

				# keyRange (if exists, it must be the first)
				keyMin, keyMax = self.getKeyRange(instrument)
				if keyMin > 0 or keyMax < 127:
					pgenData += struct.pack('<HBB', SF2.sfGenId['keyRange'], keyMin, keyMax)
					pgenNdx += 1

				# instrument (it must be the last)
				pgenData += struct.pack('<HH', SF2.sfGenId['instrument'], instNum)
				pgenNdx += 1
				instNum += 1

			instData += struct.pack('<19sBH', instrumentName.encode('ascii'), 0, ibagNdx)

			# Instrument options
			# ------------------

			genList = self.createGenList(instrument)

			if len(genList) > 0:
				# Create a global zone for this instrument
				ibagData += struct.pack('<HH', igenNdx, 0)
				ibagNdx += 1

			for gen in genList.keys():
				igenData += struct.pack('<H{}'.format(SF2.sfGenType[gen]), SF2.sfGenId[gen], genList[gen])
				igenNdx += 1

			for group in instrument['groups']:
				lovel = 0
				hivel = 127
				vel = 0
				randomRegion = False
				if self.getOpcode('RandomRegion', None, group, default = False):
					lovel = self.getOpcode('lovel', None, group, default = 0)
					hivel = self.getOpcode('hivel', None, group, default = 127)
					vel = hivel
					randomRegion = True

				repeat = True
				while repeat:
					for region in group['regions']:
						sample = self.getOpcode('sample', instrument, group, region)
						if not sample:
							continue

						channels = self.sampleList[sample][0]
						for ch in range(0, channels):
							ibagData += struct.pack('<HH', igenNdx, 0)
							ibagNdx += 1

							# Zone options
							# ------------

							# keyRange (if exists, it must be the first)
							lokey = self.getOpcode('lokey', instrument, group, region, 0)
							hikey = self.getOpcode('hikey', instrument, group, region, 127)
							if lokey > 0 or hikey < 127:
								igenData += struct.pack('<HBB', SF2.sfGenId['keyRange'], lokey, hikey)
								igenNdx += 1

							# velRange (if exists, it must be preceded only by keyRange)
							if randomRegion:
								igenData += struct.pack('<HBB', SF2.sfGenId['velRange'], vel, vel)
								igenNdx += 1
							else:
								lovel = self.getOpcode('lovel', None, group, region, 0)
								hivel = self.getOpcode('hivel', None, group, region, 127)
								if lovel > 0 or hivel < 127:
									igenData += struct.pack('<HBB', SF2.sfGenId['velRange'], lovel, hivel)
									igenNdx += 1

							# pan
							if channels == 2:
								if ch == 0:
									igenData += struct.pack('<Hh', SF2.sfGenId['pan'], -500)
								else:
									igenData += struct.pack('<Hh', SF2.sfGenId['pan'], 500)
								igenNdx += 1
							else:
								pan = self.getOpcode('pan', instrument, group, region, 0)
								if pan != 0:
									igenData += struct.pack('<Hh', SF2.sfGenId['pan'], int(pan * 5))
									igenNdx += 1

							# sampleModes
							loopMode = self.getOpcode('loop_mode', instrument, group, region, 'no_loop')
							sampleModes = 0
							if loopMode == 'loop_continuous':
								sampleModes = 1
							elif loopMode == 'loop_sustain':
								sampleModes = 3
							if sampleModes != 0:
								igenData += struct.pack('<HH', SF2.sfGenId['sampleModes'], sampleModes)
								igenNdx += 1

							# overridingRootKey
							pitch = self.getOpcode('pitch_keycenter', instrument, group, region, 60)
							if pitch != self.sampleList[sample][2]:
								igenData += struct.pack('<Hh', SF2.sfGenId['overridingRootKey'], pitch)
								igenNdx += 1

							# velocity
							ampVelTrack = self.getOpcode('amp_veltrack', instrument, group, region, 100)
							if ampVelTrack == 0:
								igenData += struct.pack('<HH', SF2.sfGenId['velocity'], 127)
								igenNdx += 1

							# other options
							genList = self.createGenList(None, group, region)
							for gen in genList.keys():
								igenData += struct.pack('<H{}'.format(SF2.sfGenType[gen]), SF2.sfGenId[gen], genList[gen])
								igenNdx += 1

							# sampleID (it must be the last)
							igenData += struct.pack('<HH', SF2.sfGenId['sampleID'], self.sampleList[sample][1] + ch)
							igenNdx += 1

						if randomRegion:
							vel -= 1
							if vel < lovel:
								repeat = False
								break

					if not randomRegion:
						repeat = False

		phdrData += struct.pack('<20sHHHIII', b'EOP', 0, 0, pbagNdx, 0, 0, 0)
		pbagData += struct.pack('<HH', pgenNdx, 0)
		pmodData = struct.pack('<HHhHH', 0, 0, 0, 0, 0)
		pgenData += struct.pack('<HH', 0, 0)
		instData += struct.pack('<20sH', b'EOI', ibagNdx)
		ibagData += struct.pack('<HH', igenNdx, 0)
		imodData = struct.pack('<HHhHH', 0, 0, 0, 0, 0)
		igenData += struct.pack('<HH', 0, 0)
		self.shdrData += struct.pack('<20sIIIIIBbHH', b'EOS', 0, 0, 0, 0, 0, 0, 0, 0, 0)

		return [[b'LIST', b'pdta'], [
			[b'phdr', phdrData],
			[b'pbag', pbagData],
			[b'pmod', pmodData],
			[b'pgen', pgenData],
			[b'inst', instData],
			[b'ibag', ibagData],
			[b'imod', imodData],
			[b'igen', igenData],
			[b'shdr', self.shdrData]
		]]

