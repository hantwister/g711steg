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

#ifndef ITOSTEGALGORITHM_HPP
#define ITOSTEGALGORITHM_HPP

#include "ItoCommon.hpp"
#include "ItoQueue.hpp"
#include "ItoOptions.hpp"
#include "../common/G711getNoisiestExtremePatternOnly.hpp"
#include "../common/InitOptions.hpp"

class ItoStegAlgorithm : public G711StegAlgorithm, public InitOptions {
	friend error_t itoParser(int key, char *arg, struct argp_state *state);
	
	private:
		static ItoStegAlgorithm *lastArgp;
		
		// Re-cache g726{sign,max}
		inline void recalcBitrateAttrs();
	
	protected:
		g726Audio g726sign, g726max;
		unsigned int g726Bitrate;
		ItoQueue *untamperedSending, *tamperedReceiving;
		
		// Generate a new ItoG711Sample instance
		virtual ItoG711Sample* newSample();
		
		// Processes a sample, updates the state provided
		virtual ItoG711Sample* processSample(G711Sample sample, g726_state_t *state);
		
		// Has a sample processed (by processSample) and adds it to the queue
		// Will update the codec state in the queue
		// If the queue doesn't yet exist, it will first be created
		virtual void postToQueue(G711Sample sample, ItoQueue **toQueue);
		
		// Transcode a single G711Sample into G726
		g726Audio runG726(g726_state_t *sourceState, G711Sample sample, g726_state_t *destState);
		
		// Get a ItoG711Sample pointer for a given index
		inline ItoG711Sample* getUntamperedSample(index_t forIndex);
		
		// Encode hidden data into a single sample
		virtual G711Sample produceTampering(ItoG711Sample *sample, steg_t hiddenData);
		
		// Recover hidden data from a single sample
		virtual length_t recoverHidden(ItoG711Sample *sample, steg_t *hiddenData);
		
		// Create a new queue, but don't initialize it
		virtual ItoQueue* allocQueue();
		
		// Reset a given queue
		virtual void resetQueue(ItoQueue *queue);
		
		// Convert a g726Audio value into a properly signed short for display
		// Assumes bitrate of this instance
		short g726signedValue(g726Audio a);
		
		// Inherited functions
		virtual G711Sample getNewlyTamperedSample(index_t forIndex, steg_t givenSteg);
		virtual G711Sample getUntamperedOut(index_t index);
		
	public:
		ItoStegAlgorithm(unsigned int g726bitrate = 40000);
	
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
		virtual error_t argp(int key, char *arg, struct argp_state *state);
		virtual struct argp_child* getArgp();
		
		virtual ~ItoStegAlgorithm() {
			if (untamperedSending)
				delete untamperedSending;
			if (tamperedReceiving)
				delete tamperedReceiving;
		}
};

#endif
