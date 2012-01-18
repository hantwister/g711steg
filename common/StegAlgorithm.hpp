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

#ifndef STEGALGORITHM_HPP
#define STEGALGORITHM_HPP

// Any array length/index should be an unsigned int
typedef unsigned int length_t;
typedef unsigned int index_t;

// For now, assume a steg algorithm won't cram more than 32 bits per sample
// LSB will contain the first secret bit; further secret bits will use increasingly more significant bits
typedef unsigned int steg_t;

/*
Class description for a simple steganography algorithm.
Implementations should behave like two queues -
- The first untampered sample pushed will be the first tampered and popped
- The first tampered sample pushed will be the first to have hidden data extracted and popped
*/
template <class S>
class StegAlgorithm {
	public:
		// should take in untampered samples and prepare to tamper them
		// samples parameter will be an array, see length parameter
		// algorithm can do whatever analysis deemed necessary on samples
		// algorithm should NOT rely on the samples pointer remaining valid post-call
		virtual void pushUntamperedSamples(const S *samples, length_t length) = 0;

		// should return number of samples ready for tampering
		// that is, number of samples that can be popped
		virtual length_t untamperedSamplesReadyForPop() = 0;

		// should return the minimum number of samples that can currently be popped
		virtual length_t minimumSamplesForPop() = 0;

		// should return the number of bits that can be embedded
		// into a given sample specified by index
		// an index of 0 refers to the next sample to be popped
		// index should not equal or exceed a value returned by untamperedSamplesReadyForPop()
		virtual length_t bitsAvailableForEncode(index_t index) = 0;

		// for the number of samples given by length parameter, should:
		// - take the next sample to be popped (FIFO - least recent to be pushed),
		// - embed the bits given by stegData parameter,
		// - insert the tampered sample into the samples array
		// - optionally provide state into the state array
		// samples, stegData and state will be arrays of size length
		// samples[0] should be made to contain the first popped sample tampered with stegData[0]
		// state[0] can be made to contain some information regarding algorithm state
		// should return the number of samples popped into the array
		virtual length_t popTamperedSamples(S *samples, const steg_t *stegData, int *state, length_t length) = 0;
		
		// should reset any algorithm state regarding samples to be tampered
		virtual void resetUntampered() = 0;

		// should take in tampered samples and prepare to recover hidden data
		// samples is an array, see length parameter
		// algorithm can do whatever analysis deemed necessary on samples
		// algorithm should NOT rely on the samples pointer remaining valid post-call
		virtual void pushTamperedSamples(const S *samples, length_t length) = 0;

		// should return the number of samples fully processed and
		// ready to recover the original hidden data
		virtual length_t recoveredDataReadyForPop() = 0;

		// for the number of samples given by length parameter, should:
		// - take the next tampered sample to be popped (FIFO - least recent to be pushed),
		// - recover the hidden bits, and place them in stegData,
		// - place the number of hidden bits recovered in bitLength
		// - optionally provide state into the state array
		// stegData, bitLength and state will be arrays of size length
		// the bitLength[0] LSBs of stegData[0] should be made to contain the hidden data of the first popped sample
		// state[0] can be made to contain some information regarding algorithm state
		// should return the number of samples popped
		virtual length_t popRecoveredData(steg_t *stegData, length_t *bitLength, int *state, length_t length) = 0;
		
		// should reset any algorithm state regarding samples with hidden data to be recovered
		virtual void resetTampered() = 0;

		virtual ~StegAlgorithm() {}
};

#endif
