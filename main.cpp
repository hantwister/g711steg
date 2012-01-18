/*
(C) 2011 Harrison Neal, Hala ElAarag.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAIN_CPP
#define MAIN_CPP

#ifndef CLASS
#error Must define CLASS to be compiled with this executable
#endif

#include <argp.h>
#include ".class.hpp"
#include "common/G711Sample.hpp"
#include "common/FileBitProvider.hpp"
#include "common/WorstNoiseBitProvider.hpp"
#include <iostream>
#include <fstream>
#include <queue>
#include <cstdlib>
#include <string.h>

// For display in usage
static const char *args_doc = "G711AUDIO OUTPUT";
static const char *doc = "Test for G711 steganography techniques";

#define ALAW_L_OPTION "alaw"
#define ALAW_S_OPTION 'a'
#define ULAW_L_OPTION "ulaw"
#define ULAW_S_OPTION 'u'
#define WORST_INPUT_L_OPTION "worst"
#define WORST_INPUT_S_OPTION 'w'
#define FILE_INPUT_L_OPTION "file"
#define FILE_INPUT_S_OPTION 'f'
#define OUTPUT_L_OPTION "output"
#define OUTPUT_S_OPTION 'o'
#define SUMMARY_L_OPTION "summary"
#define SUMMARY_S_OPTION 's'
#define DETAILED_L_OPTION "detailed"
#define DETAILED_S_OPTION 'd'

#define FILE_STR "FILE"

static struct argp_option mainArgp_opts[] = {
	// Group 0: What kind of audio:
	{ALAW_L_OPTION, ALAW_S_OPTION, 0, 0, "Assume G711AUDIO is alaw stream (default)", 0},
	{ULAW_L_OPTION, ULAW_S_OPTION, 0, 0, "Assume G711AUDIO is ulaw stream", 0},
	// Group 1: Do what with the audio:
	{WORST_INPUT_L_OPTION, WORST_INPUT_S_OPTION, 0, 0, "Embed G711AUDIO with worst-case noise scenario and write to OUTPUT (default)", 1},
	{FILE_INPUT_L_OPTION, FILE_INPUT_S_OPTION, FILE_STR, 0, "Embed G711AUDIO with FILE and write to OUTPUT", 1},
	{OUTPUT_L_OPTION, OUTPUT_S_OPTION, 0, 0, "Extract to OUTPUT a file previously embedded into G711AUDIO", 1},
	// Group 2: Summary information:
	{SUMMARY_L_OPTION, SUMMARY_S_OPTION, FILE_STR, 0, "Write a statistics summary to a file", 2},
	{DETAILED_L_OPTION, DETAILED_S_OPTION, FILE_STR, 0, "Write detailed statistics to a file", 2},
	{ 0 }
};

typedef struct mainArgsS {
	// These will reflect the arguments the program was called with (see above)
	// All will start as null or false
	// Mutually exclusive options will result in a presented error
	bool isAlaw;
	bool isUlaw;
	bool isWorst;
	char* embedFile;
	bool isOutput;
	char* summaryFile;
	char* detailedFile;
	char* audioFile;
	char* outputFile;
} mainArgs;

void checkLaw(struct argp_state *state, mainArgs *args) {
	if (args->isAlaw && args->isUlaw)
		argp_error(state, "alaw and ulaw are mutually exclusive options");
}

void checkManip(struct argp_state *state, mainArgs *args) {
	int inputs = 0, outputs = 0;
	if (args->isWorst) inputs++;
	if (args->embedFile) inputs++;
	if (args->isOutput) outputs++;
	if (inputs && outputs)
		argp_error(state, "cannot simultaneously embed and extract data from file");
	if (inputs > 1)
		argp_error(state, "can only embed one source of data at a time");
}

error_t mainParser(int key, char *arg, struct argp_state *state) {
	mainArgs *args = (mainArgs*) state->input;
	switch (key) {
		case ALAW_S_OPTION:
			args->isAlaw = true;
			checkLaw(state, args);
			return 0;
		case ULAW_S_OPTION:
			args->isUlaw = true;
			checkLaw(state, args);
			return 0;
		case WORST_INPUT_S_OPTION:
			args->isWorst = true;
			checkManip(state, args);
			return 0;
		case FILE_INPUT_S_OPTION:
			if (args->embedFile) { // Already specified?
				argp_usage(state);
			}
			args->embedFile = arg;
			checkManip(state, args);
			return 0;
		case OUTPUT_S_OPTION:
			args->isOutput = true;
			checkManip(state, args);
			return 0;
		case SUMMARY_S_OPTION:
			if (args->summaryFile) { // Already specified?
				argp_usage(state);
			}
			args->summaryFile = arg;
			return 0;
		case DETAILED_S_OPTION:
			if (args->detailedFile) { // Already specified?
				argp_usage(state);
			}
			args->detailedFile = arg;
			return 0;
		case ARGP_KEY_ARG: // A non-option key - the audio file or output file
			switch (state->arg_num) {
				case 0: args->audioFile = arg; break;
				case 1: args->outputFile = arg; break;
				default: argp_usage(state);
			}
			return 0;
		case ARGP_KEY_END: // End of non-options - check to make sure we have the audio file
			if ((!args->audioFile) || (!args->outputFile)) {
				argp_usage(state);
			}
			return 0;
		default:
			return ARGP_ERR_UNKNOWN;
	}
}

inline length_t readSamples(std::ifstream *fileIn, bool law, G711Sample *samplesOut) {
	g711Audio audioIn[SAMPLES_PER_PACKET];
	fileIn->read((char*)audioIn, SAMPLES_PER_PACKET);
	
	length_t count = fileIn->gcount();
	
	for (index_t i = 0; i < count; i++)
		samplesOut[i] = G711Sample(law, audioIn[i]);
	
	return count;
}

int main(int argc, char **argv) {
	CLASS g711steg;
	mainArgs args;
	args.isAlaw = false;
	args.isUlaw = false;
	args.isWorst = false;
	args.embedFile = NULL;
	args.isOutput = false;
	args.summaryFile = NULL;
	args.detailedFile = NULL;
	args.audioFile = NULL;
	args.outputFile = NULL;
	
	struct argp argParser = { mainArgp_opts, mainParser, args_doc, doc, g711steg.getArgp() };
	argp_parse (&argParser, argc, argv, 0, 0, &args);
	
	// Set defaults for options not chosen
	if ((!args.isAlaw) && (!args.isUlaw))
		args.isAlaw = true;
	
	if ((!args.isWorst) && (!args.embedFile) && (!args.isOutput))
		args.isWorst = true;
	
	// Open audio file
	std::ifstream audio;
	audio.open(args.audioFile, std::ios::in | std::ios::binary);
	if (audio.is_open())
		std::cout << "[Main] File " << args.audioFile << " is " << (args.isAlaw ? "a" : "u") << "law" << std::endl;
	else {
		std::cout << "[Main] Couldn't open file " << args.audioFile << std::endl;
		return 1;
	}
	
	// Open output file	
	std::ofstream output;
	output.open(args.outputFile, std::ios::out | std::ios::binary);
	if (output.is_open())
		std::cout << "[Main] Writing to " << args.outputFile << std::endl;
	else {
		std::cout << "[Main] Couldn't open file " << args.outputFile << std::endl;
		audio.close();
		return 1;
	}
	
	// If present, open stats files
	std::ofstream summaryOut, detailedOut;
	if (args.summaryFile) {
		summaryOut.open(args.summaryFile, std::ios::out);
		if (summaryOut.is_open())
			std::cout << "[Main] Writing summary to " << args.summaryFile << std::endl;
		else {
			std::cout << "[Main] Couldn't open file " << args.summaryFile << std::endl;
			audio.close();
			output.close();
			return 1;
		}
	}
	if (args.detailedFile) {
		detailedOut.open(args.detailedFile, std::ios::out);
		if (detailedOut.is_open()) {
			std::cout << "[Main] Writing details to " << args.detailedFile << std::endl;
			detailedOut << "Sample\tInput\tState\tOutput\tData\tLength\tNSR" << std::endl;
		} else {
			std::cout << "[Main] Couldn't open file " << args.detailedFile << std::endl;
			audio.close();
			output.close();
			if (args.summaryFile) summaryOut.close();
			return 1;
		}
	}
	
	// Read audio and process it
	bool law = args.isAlaw ? ALAW : ULAW;
	
	index_t sampleIndex;
	length_t sampleCount;
	G711Sample samples[SAMPLES_PER_PACKET];
	
	steg_t hiddenData[SAMPLES_PER_PACKET];
	length_t hiddenDataLength[SAMPLES_PER_PACKET];
	steg_t hiddenDataMask;
	index_t hiddenDataBitIndex;
	
	length_t processedSamples = 0;
	std::queue<G711Sample> originals;
	
	length_t processedHiddenBits = 0;
	
	int state[SAMPLES_PER_PACKET];
	
	if (args.isOutput) { // Output a file hidden in the audio
		unsigned char thisByte = 0, byteMask = 1;
	
		while (sampleCount = readSamples(&audio, law, samples)) {
			// For statistics
			for (sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++)
				originals.push(samples[sampleIndex]);
			
			g711steg.pushTamperedSamples(samples, sampleCount);
			while (sampleCount = g711steg.recoveredDataReadyForPop()) {
				if (sampleCount > SAMPLES_PER_PACKET) sampleCount = SAMPLES_PER_PACKET;
				sampleCount = g711steg.popRecoveredData(hiddenData, hiddenDataLength, state, sampleCount);
				if (!sampleCount) break;
				for (sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++) {
					processedSamples++;
					if (args.detailedFile) {
						detailedOut << processedSamples << "\t";
						detailedOut << "n/a\t";
						detailedOut << state[sampleIndex] << "\t";
						detailedOut << originals.front().uninvertedSignedSample() << "\t";
						detailedOut << (hiddenData[sampleIndex] & ((1 << hiddenDataLength[sampleIndex]) - 1)) << "\t";
						detailedOut << hiddenDataLength[sampleIndex] << "\t";
						detailedOut << "n/a" << std::endl;
					}
					originals.pop();
					
					for (hiddenDataBitIndex = 0, hiddenDataMask = 1;
						hiddenDataBitIndex < hiddenDataLength[sampleIndex];
						hiddenDataBitIndex++, hiddenDataMask <<= 1) {
							processedHiddenBits++;
							if (hiddenData[sampleIndex] & hiddenDataMask) thisByte |= byteMask;
							byteMask <<= 1;
							if (!byteMask) {
								// A full byte has been collected, push it out
								byteMask = 1;
								output.put((char) thisByte);
								thisByte = 0;
							}
						}
				}
			}
		}
	} else { // Add data into the audio
		BitProvider *bitSource;
		if (args.isWorst)
			bitSource = new WorstNoiseBitProvider(&g711steg);
		else
			bitSource = new FileBitProvider(args.embedFile);
		
		// For future verification and statistics
		std::queue<steg_t> verifyEmbedData;
		std::queue<length_t> verifyEmbedLength;
		steg_t expData, actData;
		length_t expLen, actLen;
		
		G711Sample thisOriginal, thisModified;
		linearAudio noise, signal;
		double thisNSR, NSRsum;
			
		while ((sampleCount = readSamples(&audio, law, samples)) && (bitSource->remainingBits())) {
			// For statistics
			for (sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++)
				originals.push(samples[sampleIndex]);
			
			g711steg.pushUntamperedSamples(samples, sampleCount);
			while ((g711steg.untamperedSamplesReadyForPop()) && (bitSource->remainingBits())) {
				
				sampleCount = g711steg.minimumSamplesForPop();
				if (sampleCount > SAMPLES_PER_PACKET) {
					std::cout << "[Main] Buffer length exceeded for algorithm minimum" << std::endl;
					audio.close();
					output.close();
					if (args.summaryFile) summaryOut.close();
					if (args.detailedFile) detailedOut.close();
				}
				
				// Keep in mind we're doing the minimum number of samples for a successful pop.
				// There might be a few extra 0 bits at the end of the file as a result.
				// The alternative would be to add the check for remaining bits to this for loop -
				// but then we'd not finish working on the samples and the end of the file would
				// be cut off.
				for (sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++) {
					hiddenDataMask = 1;
					hiddenDataLength[sampleIndex] = g711steg.bitsAvailableForEncode(sampleIndex);
					hiddenData[sampleIndex] = 0;
					for (hiddenDataBitIndex = 0; hiddenDataBitIndex < hiddenDataLength[sampleIndex]; hiddenDataBitIndex++) {
						processedHiddenBits++;
						if (bitSource->nextBit()) hiddenData[sampleIndex] |= hiddenDataMask;
						hiddenDataMask <<= 1;
					}
					
					// For later verification
					verifyEmbedData.push(hiddenData[sampleIndex]);
					verifyEmbedLength.push(hiddenDataLength[sampleIndex]);
				}
				
				sampleCount = g711steg.popTamperedSamples(samples, hiddenData, state, sampleCount);
				
				// Write out new samples, do statistics
				for (sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++) {
					thisOriginal = originals.front();
					originals.pop();
					thisModified = samples[sampleIndex];
					
					output.put((char) thisModified.transmissionSample());
					
					noise = thisOriginal.linearDifference(thisModified);
					signal = thisOriginal.linearSample();
					thisNSR = (1.0 * noise / signal);
					thisNSR *= thisNSR;
					NSRsum += thisNSR;
					processedSamples++;
					
					if (args.detailedFile) {
						detailedOut << processedSamples << "\t";
						detailedOut << thisOriginal.uninvertedSignedSample() << "\t";
						detailedOut << state[sampleIndex] << "\t";
						detailedOut << thisModified.uninvertedSignedSample() << "\t";
						detailedOut << (hiddenData[sampleIndex] & ((1 << hiddenDataLength[sampleIndex]) - 1)) << "\t";
						detailedOut << hiddenDataLength[sampleIndex] << "\t";
						detailedOut << thisNSR << std::endl;
					}
				}
				
				// Verify embedded data
				g711steg.pushTamperedSamples(samples, sampleCount);
				
				sampleCount = g711steg.recoveredDataReadyForPop();
				sampleCount = g711steg.popRecoveredData(hiddenData, hiddenDataLength, state, sampleCount);
				
				for (sampleIndex = 0; sampleIndex < sampleCount; sampleIndex++) {
					expData = verifyEmbedData.front();
					verifyEmbedData.pop();
					actData = hiddenData[sampleIndex];
					
					expLen = verifyEmbedLength.front();
					verifyEmbedLength.pop();
					actLen = hiddenDataLength[sampleIndex];
					
					if (expLen != actLen) {
						std::cout << "[Main] Corruption detected: expected length "
							<< expLen << "; got length " << actLen << std::endl;
						audio.close();
						output.close();
						if (args.summaryFile) summaryOut.close();
						if (args.detailedFile) detailedOut.close();
						return 1;
					} else {
						hiddenDataMask = (1 << expLen) - 1;
						expData &= hiddenDataMask;
						actData &= hiddenDataMask;
						if (expData != actData) {
							std::cout << "[Main] Corruption detected: expected data "
								<< expData << "; got data " << actData << std::endl;
							audio.close();
							output.close();
							if (args.summaryFile) summaryOut.close();
							if (args.detailedFile) detailedOut.close();
							return 1;
						}
					}
				}
			}
		}
		
		// Final stats
		if (args.summaryFile) summaryOut << "Average noise-signal ratio:\t" << std::fixed << (NSRsum / processedSamples) << std::endl;
		
		delete bitSource;
	}
	
	audio.close();
	output.close();
	if (args.summaryFile) {
		// Final stats
		summaryOut << "Average hidden bitrate b/s:\t" << std::fixed <<
			(processedHiddenBits / (processedSamples * 1.0 / SAMPLES_PER_SECOND)) << std::endl;
		
		summaryOut.close();
	}
	if (args.detailedFile) detailedOut.close();
		
	std::cout << "[Main] Finished" << std::endl;
	
	return 0;
}

#endif
