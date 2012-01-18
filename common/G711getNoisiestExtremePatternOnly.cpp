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

#ifndef G711GETNOISIESTEXTREMEPATTERNONLY_CPP
#define G711GETNOISIESTEXTREMEPATTERNONLY_CPP

#include "G711getNoisiestExtremePatternOnly.hpp"
#include <cstdlib>

// For algorithms where the noisiest pattern will be all 0s or 1s
steg_t getNoisiestExtremePatternOnly(index_t index, G711StegAlgorithm *on) {
	G711Sample thisSample = on->getUntamperedOut(index);
	length_t bitCount = on->bitsAvailableForEncode(index);
	steg_t bits = 0;
	if (bitCount > 0) {
		linearAudio lowNoise = thisSample.linearDifference(on->getNewlyTamperedSample(index, 0));
		linearAudio highNoise = thisSample.linearDifference(on->getNewlyTamperedSample(index, ~0));
		if (abs(highNoise) > abs(lowNoise)) bits = ~0;
	}
	return bits;
}

#endif
