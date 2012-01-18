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

#ifndef G711STEGALGORITHM_CPP
#define G711STEGALGORITHM_CPP

#include "G711StegAlgorithm.hpp"
#include <cstdlib>

// A naive implementation - try every bit pattern
steg_t G711StegAlgorithm::getNoisiestBitPattern(index_t index) {
	G711Sample thisSample = getUntamperedOut(index);
	length_t bitCount = bitsAvailableForEncode(index);
	steg_t bits = 0;
	if (bitCount > 0) {
		length_t options = 2;
		for (index_t i = 1; i < bitCount; i++)
			options *= 2;
		
		linearAudio highestNoise = 0;
		for (steg_t i = 0; i < options; i++) {
			linearAudio thisNoise = thisSample.linearDifference(getNewlyTamperedSample(index, i));
			if (abs(thisNoise) > abs(highestNoise)) {
				highestNoise = thisNoise;
				bits = i;
			}
		}
	}
	return bits;
}

#endif
