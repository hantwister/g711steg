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
An Approach of Covert Communication Based on the
Adaptive Steganography Scheme on Voice over IP

ISBN 978-1-61284-231-8

Authors:
- Rui Miao
- Yongfeng Huang

The authors of the above mentioned paper do not endorse this work.
*/

#ifndef MIAOG711SAMPLEGROUP_HPP
#define MIAOG711SAMPLEGROUP_HPP

#include "../common/G711Sample.hpp"
#include "../common/StegAlgorithm.hpp"
#include <vector>

// Hold the sample along with the number of bits it can store.
class MiaoG711SampleGroup {
	public:
		std::vector<G711Sample> samples;
		int mu;
		std::vector<int> deltas;
		std::vector<int> groupDelta;
		std::vector<length_t> bitCount;
		
		MiaoG711SampleGroup() {}
		
		~MiaoG711SampleGroup() {}
};

#endif
