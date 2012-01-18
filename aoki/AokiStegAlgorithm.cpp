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

#ifndef AOKISTEGALGORITHM_CPP
#define AOKISTEGALGORITHM_CPP

#include "AokiStegAlgorithm.hpp"
#include "AokiOptions.hpp"
#include <iostream>
#include <stdlib.h>

AokiStegAlgorithm* AokiStegAlgorithm::lastArgp = NULL;

error_t aokiParser(int key, char *arg, struct argp_state *state) {
	return AokiStegAlgorithm::lastArgp->argp(key, arg, state);
}

error_t AokiStegAlgorithm::argp(int key, char *arg, struct argp_state *state) {
	if (key == VARIANCE_KEY) {
		j = atoi(arg);
		bool OK = false;
		for (int i = 1; i <= 64; i*=2) if (j == i - 1) { OK = true; break; }
		if (!OK)
			argp_error(state, "%s is not a valid j - try a power of 2 less 1", arg);
		
		bitsForZeroMag();
		std::cout << "[Aoki] J: " << arg << std::endl;
		return 0;
	} else {
		return ARGP_ERR_UNKNOWN;
	}
}

struct argp_child* AokiStegAlgorithm::getArgp() {
	AokiStegAlgorithm::lastArgp = this;
	return aokiArgp;
}

#endif
