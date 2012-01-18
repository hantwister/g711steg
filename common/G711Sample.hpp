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

#ifndef G711SAMPLE_HPP
#define G711SAMPLE_HPP

#include "g72x/g72x.h"
#include <cmath>

typedef unsigned char g711Audio;
typedef int linearAudio;

#define INVERT_MASK_ALAW ((g711Audio) 0x55)
#define INVERT_MASK_ULAW ((g711Audio) 0xFF)
#define SIGN ((g711Audio) 0x80)
#define SAMPLES_PER_PACKET 160
#define SAMPLES_PER_SECOND 8000

#define ULAW true
#define ALAW false

class G711Sample {
	private:
		// True if ulaw, false otherwise
		bool law;
		// A G711 transmission
		g711Audio transmission;
		
	public:
		// By default, represents a silent alaw sample
		G711Sample(bool inLaw = ALAW, g711Audio in = INVERT_MASK_ALAW, bool isInverted = true) {
			changeValue(inLaw, in, isInverted);
		}
		
		void changeValue(bool inLaw, g711Audio in, bool isInverted) {
			law = inLaw;
			if (isInverted)
				transmission = in;
			else
				transmission = in ^ inversionMask();
		}
		
		void changeValue(short signedValue) {
			g711Audio newValue = (g711Audio) std::abs(signedValue);
			if (newValue & SIGN) newValue = 127;
			if (signedValue < 0) newValue += SIGN;
			transmission = newValue ^ inversionMask();
		}
		
		void shiftValue(short signedValue) {
			changeValue(uninvertedSignedSample() + signedValue);
		}
	
		// Returns true if a ulaw sample
		bool isUlaw() const { return law==ULAW; }
		
		// Returns true if an alaw sample
		bool isAlaw() const { return law==ALAW; }
		
		// Returns a G711 sample ready for transmission
		g711Audio transmissionSample() const { return transmission; }
		
		// Returns a G711 sample prior to inverting every other bit
		g711Audio uninvertedSample() const { return transmission ^ inversionMask(); }
		
		// Returns the inversion mask used
		unsigned char inversionMask() const { return isAlaw() ? INVERT_MASK_ALAW : INVERT_MASK_ULAW; }
		
		// Does a bitwise or on the sample uninverted
		g711Audio opOr(g711Audio const& b) const { return uninvertedSample() | b; }
		G711Sample operator| (g711Audio const& b) const {
			G711Sample toReturn(law, opOr(b), false);
			return toReturn;
		}
		G711Sample& operator|= (g711Audio const& b) {
			transmission = opOr(b) ^ inversionMask();
			return *this;
		}
		
		// Does a bitwise and on the sample uninverted
		g711Audio opAnd(g711Audio const& b) const { return uninvertedSample() & b; }
		G711Sample operator& (g711Audio const& b) const {
			G711Sample toReturn(law, opAnd(b), false);
			return toReturn;
		}
		G711Sample& operator&= (g711Audio const& b) {
			transmission = opAnd(b) ^ inversionMask();
			return *this;
		}
		
		// Does a bitwise xor on the sample uninverted
		g711Audio opXor(g711Audio const& b) const { return uninvertedSample() ^ b; }
		G711Sample operator^ (g711Audio const& b) const {
			G711Sample toReturn(law, opXor(b), false);
			return toReturn;
		}
		G711Sample& operator^= (g711Audio const& b) {
			transmission = opXor(b) ^ inversionMask();
			return *this;
		}
		
		// Does an addition on the sample uninverted
		g711Audio opAdd(g711Audio const& b) const { return uninvertedSample() + b; }
		G711Sample operator+ (g711Audio const& b) const {
			G711Sample toReturn(law, opAdd(b), false);
			return toReturn;
		}
		G711Sample& operator+= (g711Audio const& b) {
			transmission = opAdd(b) ^ inversionMask();
			return *this;
		}
		
		// Does a subtraction on the sample uninverted
		g711Audio opSub(g711Audio const& b) const { return uninvertedSample() - b; }
		G711Sample operator- (g711Audio const& b) const {
			G711Sample toReturn(law, opSub(b), false);
			return toReturn;
		}
		G711Sample& operator-= (g711Audio const& b) {
			transmission = opSub(b) ^ inversionMask();
			return *this;
		}
		
		// Returns a linear version of this sample
		linearAudio linearSample() const { return isUlaw() ? ulaw2linear(transmission) : alaw2linear(transmission); }
		
		// Returns the linear difference between this and another sample (this - other)
		// May be used to determine noise introduction
		linearAudio linearDifference(G711Sample from) const { return linearSample() - from.linearSample(); }
		
		// Returns this value uninverted and signed for display
		short uninvertedSignedSample() const {
			g711Audio orig = uninvertedSample();
			return ((orig & SIGN) ? -1 : 1) * (orig & (~SIGN));
		}
		
		~G711Sample() {}
};

#endif
