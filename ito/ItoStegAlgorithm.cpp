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
INFORMATION HIDING FOR G.711 SPEECH BASED ON SUBSTITUTION OF LEAST
SIGNIFICANT BITS AND ESTIMATION OF TOLERABLE DISTORTION

ISBN 978-1-4244-2354-5

Authors:
- Akinori Ito 
- Shunichiro Abe
- Yoiti Suzuki

The authors of the above mentioned paper do not endorse this work.
*/

#ifndef ITOSTEGALGORITHM_CPP
#define ITOSTEGALGORITHM_CPP

#include <string.h>
#include <cstdlib>
#include <iostream>
#include "ItoStegAlgorithm.hpp"

ItoStegAlgorithm::ItoStegAlgorithm(unsigned int g726bitrate) {
	g726Bitrate = g726bitrate;
	recalcBitrateAttrs();
	untamperedSending = tamperedReceiving = NULL;
}

ItoStegAlgorithm* ItoStegAlgorithm::lastArgp = NULL;

error_t itoParser(int key, char *arg, struct argp_state *state) {
	return ItoStegAlgorithm::lastArgp->argp(key, arg, state);
}

error_t ItoStegAlgorithm::argp(int key, char *arg, struct argp_state *state) {
	if (key == BITRATE_KEY) {
		g726Bitrate = atoi(arg);
		switch (g726Bitrate) {
			case 40000:
			case 32000:
			case 24000:
			case 16000:
				break;
			default:
				argp_error(state, "%s is not a valid bitrate - try {16,24,32,40}000", arg);
		}
		
		recalcBitrateAttrs();
		std::cout << "[Ito] G726 Bitrate: " << g726Bitrate << std::endl;
		return 0;
	} else {
		return ARGP_ERR_UNKNOWN;
	}
}

struct argp_child* ItoStegAlgorithm::getArgp() {
	ItoStegAlgorithm::lastArgp = this;
	return itoArgp;
}

ItoG711Sample* ItoStegAlgorithm::newSample() {
	return new ItoG711Sample();
}

ItoG711Sample* ItoStegAlgorithm::processSample(G711Sample sample, g726_state_t *state) {
	ItoG711Sample *toReturn = newSample();
	toReturn->sample = sample;
	toReturn->bits = 0;
	
	G711Sample lowTamper = sample, highTamper = sample;
	g711Audio mask = 1;
	g726Audio lowResult, highResult;
	
	g726_state_t lastLowState, stateCopy;
	
	// Run the sample without tampering to check for G726 reporting a
	// maximum delta, which would make it an unreliable indicator
	lowResult = runG726(state, lowTamper, &lastLowState);
	toReturn->result = lowResult;
	
	if (abs(g726signedValue(lowResult)) < g726sign - 1)
	{
		toReturn->maxDelta = false;
		do {
			// Continue clearing and setting bits of increasing
			// significance until G726 reports that causes a change
			lowTamper &= ~mask;
			highTamper |= mask;
			mask <<= 1;
			
			highResult = runG726(state, highTamper, &stateCopy);
			lowResult = runG726(state, lowTamper, &stateCopy);
			
			if (lowResult != highResult)
				break;
				
			// If G726 didn't report a change, we can freely manipulate
			// this bit

			lastLowState = stateCopy;
			toReturn->bits++;
		} while (toReturn->bits < 4);
	} else {
		toReturn->maxDelta = true;
	}
	
	// Copy the updated state back
	*state = lastLowState;
	
	return toReturn;
}

g726Audio ItoStegAlgorithm::runG726(g726_state_t *sourceState, G711Sample sample, g726_state_t *destState) {
	memcpy(destState, sourceState, sizeof(g726_state_t));
	int16_t a = (int16_t) sample.linearSample();
	g726Audio toReturn;
	g726_encode(destState, &toReturn, (const int16_t *)(&a), 1);
	return toReturn;
}

inline ItoG711Sample* ItoStegAlgorithm::getUntamperedSample(index_t forIndex) {
	ItoG711Sample* sample = NULL;
	if (forIndex < untamperedSamplesReadyForPop()) {
		itoSampleList::iterator it = untamperedSending->samples.begin();
		for (index_t i = 0; i < forIndex; i++) it++;
		sample = *it;
	}
	return sample;
}

G711Sample ItoStegAlgorithm::produceTampering(ItoG711Sample *sample, steg_t hiddenData) {
	unsigned char audioMask = 1;
	steg_t stegMask = 1;
	G711Sample toReturn = sample->sample;
	for (index_t i = 0; i < sample->bits; i++) {
		if (hiddenData & stegMask) {
			toReturn |= audioMask;
		} else {
			toReturn &= ~audioMask;
		}
		
		audioMask <<= 1;
		stegMask <<= 1;
	}
	return toReturn;
}

length_t ItoStegAlgorithm::recoverHidden(ItoG711Sample *sample, steg_t *hiddenData) {
	unsigned char audioMask = 1;
	steg_t stegMask = 1;
	g711Audio a = sample->sample.uninvertedSample();
	*hiddenData = 0;
	for (index_t i = 0; i < sample->bits; i++) {
		if (a & audioMask)
			*hiddenData |= stegMask;
			
		audioMask <<= 1;
		stegMask <<= 1;
	}
	return sample->bits;
}

void ItoStegAlgorithm::postToQueue(G711Sample sample, ItoQueue **toQueue) {
	if (*toQueue == NULL) {
		*toQueue = allocQueue();
		resetQueue(*toQueue);
	}
	(*toQueue)->samples.push_back(processSample(sample, &((*toQueue)->lowerCodec)));
}

inline void ItoStegAlgorithm::recalcBitrateAttrs() {
	switch (g726Bitrate) {
		case 40000: g726sign = 16; break;
		case 32000: g726sign = 8; break;
		case 24000: g726sign = 4; break;
		case 16000: g726sign = 2; break;
	}
	g726max = g726sign * 2;
}

void ItoStegAlgorithm::pushUntamperedSamples(const G711Sample *samples, length_t length) {
	for (index_t i = 0; i < length; i++)
		postToQueue(samples[i], &untamperedSending);
}

length_t ItoStegAlgorithm::untamperedSamplesReadyForPop() {
	return untamperedSending->samples.size();
}

length_t ItoStegAlgorithm::minimumSamplesForPop() {
	return untamperedSending->samples.empty() ? 0 : 1;
}

length_t ItoStegAlgorithm::bitsAvailableForEncode(index_t index) {
	ItoG711Sample* sample = getUntamperedSample(index);
	if (sample == NULL) return 0;	
	return sample->bits;
}

length_t ItoStegAlgorithm::popTamperedSamples(G711Sample *samples, const steg_t *stegData, int *state, length_t length) {
	length_t size = untamperedSamplesReadyForPop();
	if (length > size) length = size;
	
	for (index_t i = 0; i < length; i++) {
		ItoG711Sample* sample = untamperedSending->samples.front();
		
		state[i] = g726signedValue(sample->result);
		samples[i] = produceTampering(sample, stegData[i]);
		
		untamperedSending->samples.pop_front();
	}
	
	return length;
}

ItoQueue* ItoStegAlgorithm::allocQueue() {
	return new ItoQueue();
}

void ItoStegAlgorithm::resetQueue(ItoQueue *queue) {
	g726_init(&(queue->lowerCodec), g726Bitrate, G726_ENCODING_LINEAR, G726_PACKING_NONE);
	queue->samples.clear();
}

void ItoStegAlgorithm::resetUntampered() {
	resetQueue(untamperedSending);
}

void ItoStegAlgorithm::pushTamperedSamples(const G711Sample *samples, length_t length) {
	for (index_t i = 0; i < length; i++)
		postToQueue(samples[i], &tamperedReceiving);
}

length_t ItoStegAlgorithm::recoveredDataReadyForPop() {
	return tamperedReceiving->samples.size();
}

length_t ItoStegAlgorithm::popRecoveredData(steg_t *stegData, length_t *bitLength, int *state, length_t length) {
	length_t size = recoveredDataReadyForPop();
	if (length > size) length = size;
	
	for (index_t i = 0; i < length; i++) {
		ItoG711Sample* sample = tamperedReceiving->samples.front();
		
		state[i] = g726signedValue(sample->result);
		bitLength[i] = recoverHidden(sample, &(stegData[i]));
		
		tamperedReceiving->samples.pop_front();
	}
	
	return length;
}

void ItoStegAlgorithm::resetTampered() {
	resetQueue(tamperedReceiving);
}

G711Sample ItoStegAlgorithm::getNewlyTamperedSample(index_t forIndex, steg_t givenSteg) {
	ItoG711Sample *sample = getUntamperedSample(forIndex);
	if (sample == NULL) return G711Sample();
	return produceTampering(sample, givenSteg);
}

G711Sample ItoStegAlgorithm::getUntamperedOut(index_t index) {
	ItoG711Sample *sample = getUntamperedSample(index);
	if (sample == NULL) return G711Sample();
	return sample->sample;
}

short ItoStegAlgorithm::g726signedValue(g726Audio a) {
	if (a < g726sign) return a;
	else return a + 1 - g726max;
}

#endif
