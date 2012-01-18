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
A Semi-Lossless Steganography Technique for G.711 Telephony Speech

ISBN 978-0-7695-4222-5

Author:
- Naofumi Aoki

The author of the above mentioned paper does not endorse this work.
*/

#ifndef AOKISTEGALGORITHM_HPP
#define AOKISTEGALGORITHM_HPP

#include "../common/G711StegAlgorithm.hpp"
#include "../common/InitOptions.hpp"
#include "AokiOptions.hpp"
#include <list>
#include <cmath>

typedef std::list<G711Sample> sampleList;

class AokiStegAlgorithm : public G711StegAlgorithm, public InitOptions {
	friend error_t aokiParser(int key, char *arg, struct argp_state *state);
	
	private:
		static AokiStegAlgorithm *lastArgp;
		sampleList untamperedSending, tamperedReceiving;
		g711Audio j;
		length_t bitsForJ;
		
		void bitsForZeroMag() {
			bitsForJ = 1;
			g711Audio tmp = j;
			while (tmp) {
				tmp >>= 1;
				bitsForJ++;
			}
		}
	
	protected:		
		// Inherited functions
		G711Sample getNewlyTamperedSample(index_t forIndex, steg_t givenSteg) {
			G711Sample toReturn = getUntamperedOut(forIndex);
			short signedSample = toReturn.uninvertedSignedSample();
			int absSignedSample = std::abs(signedSample);
			
			if (signedSample == 0) { // can have bits embedded (values [-j,+j])
				toReturn = G711Sample(
					toReturn.isAlaw() ? ALAW : ULAW,
					(givenSteg & j) + ((givenSteg & (j+1)) ? SIGN : 0),
					false);
			} else { // push up by j
				if (absSignedSample + j >= SIGN) // overflow
					toReturn = G711Sample(
						toReturn.isAlaw() ? ALAW : ULAW,
						(signedSample/absSignedSample == -1 ? SIGN : 0) + (SIGN - 1),
						false);
				else // no overflow
					toReturn += j;
			}
			
			return toReturn;
		}
		
		G711Sample getUntamperedOut(index_t index) {
			if (index >= untamperedSending.size())
				return G711Sample();
			
			sampleList::iterator it = untamperedSending.begin();
			for (index_t i = 0; i < index; i++) it++;
			return *it;
		}
		
	public:
		AokiStegAlgorithm() {
			j = 0;
			bitsForZeroMag();
		}
	
		// Inherited functions - G711StegAlgorithm
		steg_t getNoisiestBitPattern(index_t index) {
			return j + ((j+1) * (getUntamperedOut(index).uninvertedSample() & SIGN ? 0 : 1));
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
			if (getUntamperedOut(index).uninvertedSignedSample() == 0)
				return bitsForJ;
			else
				return 0;
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
				G711Sample sample = tamperedReceiving.front();
				if (std::abs(sample.uninvertedSignedSample()) <= j) {
					bitLength[i] = bitsForJ;
					g711Audio uninverted = sample.uninvertedSample();
					stegData[i] = (uninverted & j) + ((uninverted & SIGN) ? (j+1) : 0);
				} else {
					bitLength[i] = 0;
					stegData[i] = 0;
				}
				
				state[i] = 0;
				tamperedReceiving.pop_front();
			}
			return length;
		}
		
		void resetTampered() {
			tamperedReceiving.clear();
		}
		
		// Inherited functions - InitOptions
		error_t argp(int key, char *arg, struct argp_state *state);
		
		struct argp_child* getArgp();
		
		virtual ~AokiStegAlgorithm() {}
};

#endif
