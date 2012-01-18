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

// TODO: This one probably should be cleaned up a little bit before the others.

#ifndef MIAOSTEGALGORITHM_CPP
#define MIAOSTEGALGORITHM_CPP

#include <string.h>
#include <cstdlib>
#include <iostream>
#include "MiaoStegAlgorithm.hpp"

miaoGroup MiaoStegAlgorithm::groups[] = {
	{	-256,	-128,	4 },
	{	-127,	-64,	4 },
	{	-63,	-32,	4 },
	{	-31,	-16,	4 },
	{	-15,	-8,		3 },
	{	-7,		-4,		2 },
	{	-3,		-2,		1 },
	{	-1,		1,		0 },
	{	2,		3,		1 },
	{	4,		7,		2 },
	{	8,		15,		3 },
	{	16,		31,		4 },
	{	32,		63,		4 },
	{	64,		127,	4 },
	{	128,	256,	4 },
	{	0,		0,		0 }
};

MiaoStegAlgorithm* MiaoStegAlgorithm::lastArgp = NULL;

error_t miaoParser(int key, char *arg, struct argp_state *state) {
	return MiaoStegAlgorithm::lastArgp->argp(key, arg, state);
}

error_t MiaoStegAlgorithm::argp(int key, char *arg, struct argp_state *state) {
	if (key == KVAR_KEY) {
		k = atoi(arg);
		if (k < 1 || k > 79)
			argp_error(state, "%s is not a valid k - try 1-79", arg);
		
		std::cout << "[Miao] K: " << arg << std::endl;
		return 0;
	} else if (key == LAMBDA_KEY) {
		maxLambda = atoi(arg);
		if (maxLambda < 8 || maxLambda > 127)
			argp_error(state, "%s is not a valid lambda - try 8-127", arg);
		
		std::cout << "[Miao] Max Lambda: " << arg << std::endl;
		return 0;
	} else {
		return ARGP_ERR_UNKNOWN;
	}
}

struct argp_child* MiaoStegAlgorithm::getArgp() {
	MiaoStegAlgorithm::lastArgp = this;
	return miaoArgp;
}

void MiaoStegAlgorithm::process(unprocessedList *src, processedList *dest) {
	length_t nv = n();
	index_t midv = mid();
	while (src->size() >= nv) {
		MiaoG711SampleGroup staging;
		for (index_t i = 0; i < nv; i++) {
			staging.samples.push_back(src->front());
			src->pop_front();
		}
		
		if (staging.samples.at(midv).uninvertedSignedSample() == -7)
			staging.mu = 1;
		
		int mu = 0;
		for (index_t i = 0; i < nv; i++) {
				mu += staging.samples.at(i).uninvertedSignedSample();
		}
		mu = (int)floor(((double)mu) / nv);
		staging.mu = mu;
		
		int tU = mu, tL = mu;
		for (index_t i = 0; i < nv; i++) {
			if (i != midv) {
				G711Sample s = staging.samples.at(i);
				int delta = mu - s.uninvertedSignedSample();
				for (miaoGroup *group = groups; group->deltaLow != 0; group++) {
					if (delta >= group->deltaLow && delta <= group->deltaHigh) {
						tU += group->deltaHigh;
						tL += group->deltaLow;
						
						staging.bitCount.push_back(
							(std::abs(mu - group->deltaHigh) <= maxLambda ||
							std::abs(mu - group->deltaLow) <= maxLambda) ?
								group->bitsAllowed : 0);
						
						staging.groupDelta.push_back((delta >= 0) ? group->deltaLow : group->deltaHigh);
						
						break;
					}
				}
				staging.deltas.push_back(delta);
			}
		}
		
		if (std::abs(tU) > maxLambda || std::abs(tL) > maxLambda)
			staging.bitCount.clear();
			
		dest->push_back(staging);
	}
}

void MiaoStegAlgorithm::pushUntamperedSamples(const G711Sample *samples, length_t length) {
	for (index_t i = 0; i < length; i++) untamperedUnprocessed.push_back(samples[i]);
	process(&untamperedUnprocessed, &untamperedProcessed);
}

length_t MiaoStegAlgorithm::untamperedSamplesReadyForPop() {
	return untamperedProcessed.size() * n();
}

length_t MiaoStegAlgorithm::minimumSamplesForPop() {
	return untamperedProcessed.empty() ? 0 : n();
}

length_t MiaoStegAlgorithm::bitsAvailableForEncode(index_t index) {
	index_t whichGroup = index / n();
	index_t whichItem = index % n();
	
	if (whichGroup >= untamperedProcessed.size())
		return 0;
	
	if (whichItem == mid())
		return 0;
	
	processedList::iterator it = untamperedProcessed.begin();
	for (index_t i = 0; i < whichGroup; i++) it++;
	
	if (it->bitCount.empty())
		return 0;
	
	return it->bitCount.at(whichItem - (whichItem > mid() ? 1 : 0));
}

length_t MiaoStegAlgorithm::popTamperedSamples(G711Sample *samples, const steg_t *stegData, int *state, length_t length) {
	length_t size = untamperedSamplesReadyForPop();
	if (length > size) length = size;
	
	length /= n();
	
	for (index_t i = 0; i < length; i++) {
		if (!untamperedProcessed.front().bitCount.empty()) {
			int mu = untamperedProcessed.front().mu;
			int deltaSums = 0;
			for (index_t s = 0; s < n(); s++) {
				if (s != mid()) {
					index_t s_adj = s - (s > mid() ? 1 : 0);
					int groupDelta = untamperedProcessed.front().groupDelta.at(s_adj);
					int bits = untamperedProcessed.front().bitCount.at(s_adj);
					int newDelta = groupDelta + ((groupDelta/std::abs(groupDelta)) * (stegData[i*n()+s] & ((1 << bits) - 1)));
					state[i*n()+s] = 0;
					deltaSums += newDelta;
					untamperedProcessed.front().samples.at(s).changeValue(mu - newDelta);
				}
			}
			untamperedProcessed.front().samples.at(mid()).changeValue(mu + deltaSums);
		}
		
		for (index_t s = 0; s < n(); s++) samples[i*n()+s] = untamperedProcessed.front().samples.at(s);
		
		untamperedProcessed.pop_front();
	}
	
	length *= n();
	
	return length;
}

void MiaoStegAlgorithm::resetUntampered() {
	untamperedProcessed.clear();
	untamperedUnprocessed.clear();
}

void MiaoStegAlgorithm::pushTamperedSamples(const G711Sample *samples, length_t length) {
	for (index_t i = 0; i < length; i++) tamperedUnprocessed.push_back(samples[i]);
	process(&tamperedUnprocessed, &tamperedProcessed);
}

length_t MiaoStegAlgorithm::recoveredDataReadyForPop() {
	return tamperedProcessed.size()*n();
}

length_t MiaoStegAlgorithm::popRecoveredData(steg_t *stegData, length_t *bitLength, int *state, length_t length) {
	length_t size = recoveredDataReadyForPop();
	if (length > size) length = size;
	
	length /= n();
	
	for (index_t i = 0; i < length; i++) {
		if (!tamperedProcessed.front().bitCount.empty()) {
			for (index_t s = 0; s < n(); s++) {
				if (s != mid()) {
					index_t s_adj = s - (s > mid() ? 1 : 0);
					bitLength[i*n()+s] = tamperedProcessed.front().bitCount.at(s_adj);
					int groupDelta = tamperedProcessed.front().groupDelta.at(s_adj);
					int thisDelta = tamperedProcessed.front().deltas.at(s_adj);
					stegData[i*n()+s] = (thisDelta >= 0 ? thisDelta - groupDelta : groupDelta - thisDelta);
					stegData[i*n()+s] &= (1 << bitLength[i*n()+s]) - 1;
				} else {
					bitLength[i*n()+s] = 0;
					stegData[i*n()+s] = 0;
				}
				state[i*n()+s] = 0;
			}
		} else {
			for (index_t s = 0; s < n(); s++) {
				stegData[i*n()+s] = 0;
				bitLength[i*n()+s] = 0;
				state[i*n()+s] = 0;
			}
		}
		
		tamperedProcessed.pop_front();
	}
	
	length *= n();
	
	return length;
}

void MiaoStegAlgorithm::resetTampered() {
	tamperedProcessed.clear();
	tamperedUnprocessed.clear();
}

G711Sample MiaoStegAlgorithm::getNewlyTamperedSample(index_t forIndex, steg_t givenSteg) {
	index_t whichGroup = forIndex / n();
	index_t whichItem = forIndex % n();
	
	if (whichGroup >= untamperedProcessed.size())
		return G711Sample();
	
	processedList::iterator it = untamperedProcessed.begin();
	for (index_t i = 0; i < whichGroup; i++) it++;
	
	G711Sample orig = it->samples.at(whichItem);
	
	if (whichItem == mid())
		return orig;	// Fudge it. This method is only used to check best candidates
						// for noise. For the middle number, we don't really have any
						// control there; we need to know the other values first.
	
	if (it->bitCount.empty()) // No changes allowed - return the original
		return orig;
	
	index_t indexLessMid = whichItem - (whichItem > mid() ? 1 : 0);
	
	length_t bits = it->bitCount.at(indexLessMid);
	int groupDelta = it->groupDelta.at(indexLessMid);
	
	return G711Sample(
		orig.isAlaw() ? ALAW : ULAW,
		it->mu - (groupDelta + ((groupDelta/std::abs(groupDelta)) * (givenSteg & ((1 << bits) - 1)))),
		false);
}

G711Sample MiaoStegAlgorithm::getUntamperedOut(index_t index) {
	index_t whichGroup = index / n();
	index_t whichItem = index % n();
	
	if (whichGroup >= untamperedProcessed.size())
		return G711Sample();
	
	processedList::iterator it = untamperedProcessed.begin();
	for (index_t i = 0; i < whichGroup; i++) it++;
	
	return it->samples.at(whichItem);
}

#endif
