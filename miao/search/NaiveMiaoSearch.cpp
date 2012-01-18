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

/*
This work is based on the following paper:
An Approach of Covert Communication Based on the
Adaptive Steganography Scheme on Voice over IP

ISBN 978-1-61284-231-8

Authors:
- Rui Miao
- Yongfeng Huang

The authors of the above mentioned paper do not endorse this work.
*/

#include <argp.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "../../common/G711Sample.hpp"

// ----- Usage, Arguments Handling -----

#define LOW_K_OPTION "lowerk"
#define LOW_K_KEY 'l'
#define HIGH_K_OPTION "upperk"
#define HIGH_K_KEY 'u'

static const char *miaoArgsDoc = "G711-ALAW-AUDIO";
static const char *miaoDoc = "Naive search for Miao/Huang-encoded audio";

typedef struct miaoDetectArgsS {
	unsigned int lowK, highK;
	char* audioFile;
} miaoDetectArgs;

error_t miaoDetectParser (int key, char *arg, struct argp_state *state) {
	miaoDetectArgs *args = (miaoDetectArgs*) state->input;
	switch (key) {
		case LOW_K_KEY:
			args->lowK = atoi(arg);
			return 0;
		case HIGH_K_KEY:
			args->highK = atoi(arg);
			return 0;
		case ARGP_KEY_ARG: // A non-option key - the audio file or output file
			switch (state->arg_num) {
				case 0: args->audioFile = arg; break;
				default: argp_usage(state);
			}
			return 0;
		case ARGP_KEY_END: // End of non-options - check to make sure we have the audio file and ks are valid
			if (!args->audioFile)
				argp_usage(state);
			if (args->lowK == -1 || args->highK == -1)
				argp_usage(state);
			if (args->lowK > args->highK)
				argp_error(state, "incorrect order for k values");
			if (args->lowK < 1 || args->lowK > 79)
				argp_error(state, "invalid lower k - try 1-79");
			if (args->highK < 1 || args->highK > 79)
				argp_error(state, "invalid upper k - try 1-79");
			return 0;
		default:
			return ARGP_ERR_UNKNOWN;
	}
}

static struct argp_option miaoDetectArgp_opts[] = { // options
	{LOW_K_OPTION, LOW_K_KEY, LOW_K_OPTION, 0, "Lowest k value to try"},
	{HIGH_K_OPTION, HIGH_K_KEY, HIGH_K_OPTION, 0, "Highest k value to try"},
	{ 0 }
};

static struct argp miaoDetectArgp_base = { // parsers
	miaoDetectArgp_opts, // options
	miaoDetectParser, // parsing function
	miaoArgsDoc, // one non-option argument
	miaoDoc // brief description
};

// ----- Program -----

typedef struct kqueueS {
	int sum;
	unsigned int hits, misses;
} kqueue;

int main(int argc, char **argv) {
	miaoDetectArgs args;
	args.lowK = args.highK = -1;
	args.audioFile = NULL;
	argp_parse (&miaoDetectArgp_base, argc, argv, 0, 0, &args);
	
	// Open audio file
	std::ifstream audio;
	audio.open(args.audioFile, std::ios::in | std::ios::binary);
	if (!audio.is_open()) {
		std::cout << "Couldn't open file " << args.audioFile << std::endl;
		return 1;
	}
	
	// Create queues for the various k values
	std::vector<kqueue> kqueues;
	for (unsigned int k = args.lowK; k <= args.highK; k++)
		kqueues.push_back({0,0,0});
	
	unsigned int samplesProcessed = 0;
	g711Audio audioIn;
	G711Sample sample;
	
	audio.read((char*)&audioIn, 1);
	while (audio.gcount()) {
		samplesProcessed++;
		sample = G711Sample(ALAW, audioIn);
		short signedSample = sample.uninvertedSignedSample();
		for (unsigned int i = 0, k = args.lowK; k <= args.highK; i++, k++) {
			kqueues.at(i).sum += signedSample;
			if (samplesProcessed % (2*k + 1) == 0) {
				if (kqueues.at(i).sum % (2*k + 1) == 0)
					kqueues.at(i).hits++;
				else
					kqueues.at(i).misses++;
				
				kqueues.at(i).sum = 0;
			}
		}
		
		audio.read((char*)&audioIn, 1);
	}
	
	for (unsigned int i = 0, k = args.lowK; k <= args.highK; i++, k++)
		printf("k = %i : %i hits (%i%%)\n", k, kqueues.at(i).hits,
			kqueues.at(i).hits * 100 / (kqueues.at(i).hits + kqueues.at(i).misses));
}
