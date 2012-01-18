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

#ifndef MIAOSTEGALGORITHM_HPP
#define MIAOSTEGALGORITHM_HPP

#include "MiaoOptions.hpp"
#include "MiaoG711SampleGroup.hpp"
#include "../common/G711StegAlgorithm.hpp"
#include "../common/G711getNoisiestExtremePatternOnly.hpp"
#include "../common/InitOptions.hpp"
#include <list>

typedef std::list<G711Sample> unprocessedList;
typedef std::list<MiaoG711SampleGroup> processedList;

typedef struct miaoGroupS {
	short deltaLow, deltaHigh;
	length_t bitsAllowed;
} miaoGroup;

class MiaoStegAlgorithm : public G711StegAlgorithm, public InitOptions {
	friend error_t miaoParser(int key, char *arg, struct argp_state *state);
	
	private:
		static MiaoStegAlgorithm *lastArgp;
		static miaoGroup groups[];
		unprocessedList untamperedUnprocessed, tamperedUnprocessed;
		processedList untamperedProcessed, tamperedProcessed;
		length_t k;
		g711Audio maxLambda;
		
		void process(unprocessedList *src, processedList *dest);
	
	protected:
		// Inherited functions
		virtual G711Sample getNewlyTamperedSample(index_t forIndex, steg_t givenSteg);
		virtual G711Sample getUntamperedOut(index_t index);
		
	public:
		MiaoStegAlgorithm(length_t inK = 3, g711Audio inLambda = 60) :
			k(inK), maxLambda(inLambda) {}
			
		length_t n() { return k*2 + 1; }
		index_t mid() { return k; }
	
		// Inherited functions - G711StegAlgorithm
		virtual steg_t getNoisiestBitPattern(index_t index) {
			return getNoisiestExtremePatternOnly(index, this);
		}
		virtual void pushUntamperedSamples(const G711Sample *samples, length_t length);
		virtual length_t untamperedSamplesReadyForPop();
		virtual length_t minimumSamplesForPop();
		virtual length_t bitsAvailableForEncode(index_t index);
		virtual length_t popTamperedSamples(G711Sample *samples, const steg_t *stegData, int *state, length_t length);
		virtual void resetUntampered();
		virtual void pushTamperedSamples(const G711Sample *samples, length_t length);
		virtual length_t recoveredDataReadyForPop();
		virtual length_t popRecoveredData(steg_t *stegData, length_t *bitLength, int *state, length_t length);
		virtual void resetTampered();
		
		// Inherited functions - InitOptions
		error_t argp(int key, char *arg, struct argp_state *state);
		virtual struct argp_child* getArgp();
		
		virtual ~MiaoStegAlgorithm() {}
};

#endif
