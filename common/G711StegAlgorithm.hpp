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

#ifndef G711STEGALGORITHM_HPP
#define G711STEGALGORITHM_HPP

#include "StegAlgorithm.hpp"
#include "G711Sample.hpp"

// Provides some naive defaults for a G711 steganography algorithm
class G711StegAlgorithm : public StegAlgorithm<G711Sample> {
	friend steg_t getNoisiestExtremePatternOnly(index_t index, G711StegAlgorithm *on);
	friend class WorstNoiseBitProvider;
	
	protected:
		// Should return the given sample tampered with the given steg data
		virtual G711Sample getNewlyTamperedSample(index_t forIndex, steg_t givenSteg) = 0;
		
		// Should return the given untampered sample in the list
		virtual G711Sample getUntamperedOut(index_t index) = 0;
	
	public:
		// Should return the noisiest bit pattern that can be encoded in a sample
		// That is, whichever bit pattern will cause the highest deviation from
		// the original sample should be returned
		// Works on samples not yet tampered, does not modify
		virtual steg_t getNoisiestBitPattern(index_t index);
};

#endif
