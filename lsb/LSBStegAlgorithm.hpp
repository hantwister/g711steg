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

#ifndef LSBSTEGALGORITHM_HPP
#define LSBSTEGALGORITHM_HPP

#include "../common/G711StegAlgorithm.hpp"
#include "../common/InitOptions.hpp"
#include <list>

static struct argp_child lsbArgp[] = {
	{ 0 }
};

class LSBStegAlgorithm : public G711StegAlgorithm, public InitOptions {
	private:
		std::list<G711Sample> untamperedSending, tamperedReceiving;
	
	protected:
		// Inherited functions
		G711Sample getNewlyTamperedSample(index_t forIndex, steg_t givenSteg) {
			G711Sample toReturn = getUntamperedOut(forIndex);
			if (givenSteg & 1)
				toReturn |= (g711Audio)1;
			else
				toReturn &= (g711Audio)~1;
			return toReturn;
		}
		
		G711Sample getUntamperedOut(index_t index) {
			if (index >= untamperedSending.size())
				return G711Sample();
			
			std::list<G711Sample>::iterator it = untamperedSending.begin();
			for (index_t i = 0; i < index; i++) it++;
			return *it;
		}
		
	public:
		LSBStegAlgorithm() {}
	
		// Inherited functions - G711StegAlgorithm
		steg_t getNoisiestBitPattern(index_t index) {
			return getUntamperedOut(index).uninvertedSample() & 1 ? 0 : 1;
		}
		
		void pushUntamperedSamples(const G711Sample *samples, length_t length) {
			for (index_t i = 0; i < length; i++) untamperedSending.push_back(samples[i]);
		}
		
		length_t untamperedSamplesReadyForPop() {
			return untamperedSending.size();
		}
		
		length_t minimumSamplesForPop() {
			return untamperedSending.empty() ? 0 : 1;
		}
		
		length_t bitsAvailableForEncode(index_t index) {
			return 1;
		}
		
		length_t popTamperedSamples(G711Sample *samples, const steg_t *stegData, int *state, length_t length) {
			length_t size = untamperedSending.size();
			if (length > size) length = size;
			for (index_t i = 0; i < length; i++) {
				samples[i] = getNewlyTamperedSample(0, stegData[i]);
				untamperedSending.pop_front();
				state[i] = 0;
			}
			return length;
		}
		
		void resetUntampered() {
			untamperedSending.clear();
		}
		
		void pushTamperedSamples(const G711Sample *samples, length_t length) {
			for (index_t i = 0; i < length; i++) tamperedReceiving.push_back(samples[i]);
		}
		
		length_t recoveredDataReadyForPop() {
			return tamperedReceiving.size();
		}
		
		length_t popRecoveredData(steg_t *stegData, length_t *bitLength, int *state, length_t length) {
			length_t size = tamperedReceiving.size();
			if (length > size) length = size;
			for (index_t i = 0; i < length; i++) {
				bitLength[i] = 1;
				stegData[i] = tamperedReceiving.front().uninvertedSample() & 1;
				state[i] = 0;
				tamperedReceiving.pop_front();
			}
			return length;
		}
		
		void resetTampered() {
			tamperedReceiving.clear();
		}
		
		// Inherited functions - InitOptions
		error_t argp(int key, char *arg, struct argp_state *state) {
			return ARGP_ERR_UNKNOWN;
		}
		
		struct argp_child* getArgp() {
			return lsbArgp;
		}
		
		virtual ~LSBStegAlgorithm() {}
};

#endif
