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

#ifndef WORSTNOISEBITPROVIDER_HPP
#define WORSTNOISEBITPROVIDER_HPP

#include <list>
#include "BitProvider.hpp"
#include "G711StegAlgorithm.hpp"

// This class will "expect" that the flow of operations will
// go as follows:
// - Optionally, new untampered samples will be pushed onto g711steg.
// - The code using this class will call minimumSamplesForPop() on g711steg.
// - The code using this class will call bitsAvailableForEncode() on g711steg
//   for the number of samples noted by minimumSamplesForPop(), getting a sum
//   of bits available over the minimum allowed popped samples.
// - The code using this class will call nextBit() a number of times equal to
//   the sum mentioned in the last step.
// - The code will pop the samples in question from g711steg.
// - Repeat.

// The implementation will always return bits intended for the next available
// popped samples. Not adhering to the above will result in bits for the wrong
// samples being returned.

// The implementation will assume g711steg will not be deallocated while it
// is being used. The implementation will not attempt to deallocate g711steg.
class WorstNoiseBitProvider : public BitProvider {
	private:
		G711StegAlgorithm *g711steg;
		steg_t hiddenData[SAMPLES_PER_PACKET];
		length_t hiddenDataLength[SAMPLES_PER_PACKET];
		length_t samples;
		steg_t currentMask;
		index_t currentSample, currentMaskIndex;
		
		inline bool loadMoreSamples();
	
	public:
		WorstNoiseBitProvider(G711StegAlgorithm *g711steg) :
			g711steg(g711steg), samples(0), currentMask(0),
			currentSample(0), currentMaskIndex(0) {}
		
		length_t remainingBits() { return ~0; }
		
		bool nextBit();
		
		virtual ~WorstNoiseBitProvider() {}
};

#endif
