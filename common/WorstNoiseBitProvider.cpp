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

#ifndef WORSTNOISEBITPROVIDER_CPP
#define WORSTNOISEBITPROVIDER_CPP

#include "WorstNoiseBitProvider.hpp"

bool WorstNoiseBitProvider::nextBit() {
	// Given the assumptions covered in the .hpp for this class,
	// we don't want to pre-emptively load in samples at the end
	// of the method call. We must load in samples only when this
	// method requires it at the last moment.
	
	// Consider if we tried pre-emptively loading samples:
	// - g711steg is given samples by some code
	// - this class obtains samples the first time this function is
	//   called
	// - some code calls this function the appropriate number of times
	// - on the last call, we notice we gave out the last bit, and try
	//   to load in more samples, but the code calling us and g711steg
	//   hasn't given g711steg any additional samples yet, so we fail
	
	// As such, here we grab samples first and foremost if we need them,
	// and do any cleanup we can aside from grabbing more samples at the
	// end.
	
	if (! (currentSample < samples)) { // We're out of samples. Get the next set.
		samples = g711steg->minimumSamplesForPop(); // Check that there actually are samples.
		if (!samples) return false;
		
		for (index_t i = 0; i < samples; i++) {
			hiddenDataLength[i] = g711steg->bitsAvailableForEncode(i);
			hiddenData[i] = g711steg->getNoisiestBitPattern(i);
		}
		
		currentSample = 0;
		currentMask = 1;
		currentMaskIndex = 0;
	}
	
	// At this point, either:
	// - We just loaded the next set of samples (safe to continue)
	//   -OR-
	// - This is a repeat call to nextBit() on the same set of samples,
	//   and because we'll clean up at the end (enough for the next call
	//   to recognize if more samples are needed), safe to continue
	
	bool toReturn = false;
	if (hiddenData[currentSample] & currentMask) toReturn = true;
	
	// Increment the mask/bit-index to be used next time.
	currentMask <<= 1;
	currentMaskIndex++;
	
	if (! (currentMaskIndex < hiddenDataLength[currentSample])) { // Out of bits for this sample?
		// Move to the next sample. If there is no next sample
		// (currentSample == samples), we'll take care of it
		// on the next call.
		currentSample++;
		currentMask = 1;
		currentMaskIndex = 0;
	}
	
	return toReturn;
}

#endif
