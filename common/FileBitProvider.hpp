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

#ifndef FILEBITPROVIDER_HPP
#define FILEBITPROVIDER_HPP

#include "BitProvider.hpp"
#include <iostream>
#include <fstream>

class FileBitProvider : public BitProvider {
	private:
		std::ifstream inputFile;
		unsigned char currentByte, currentMask;
		length_t remainingBytes;
	
	public:
		FileBitProvider(char *fileName) {
			// Open the file at the end
			inputFile.open(fileName, std::ios::in | std::ios::binary | std::ios::ate);
			// Get the length of the file
			remainingBytes = inputFile.tellg();
			// Go back to the beginning
			inputFile.seekg(0, std::ios::beg);
			
			currentByte = currentMask = 0;
			
			std::cout << "[FileBitProvider] " << fileName << " opened" << std::endl;
		}
		
		length_t remainingBits() {
			length_t toReturn = remainingBytes * 8;
			if (currentMask) {
				unsigned char tmp = currentMask;
				while (tmp) {
					toReturn++;
					tmp <<= 1;
				}
			}
			return toReturn;
		}
		
		bool nextBit() {
			if (!currentMask) {
				if (inputFile.good())
					currentByte = (unsigned char) inputFile.get();
				
				if (inputFile.good()) {
					remainingBytes--;
					currentMask = 1;
				} else {
					std::cout << "[FileBitProvider] EOF" << std::endl;
					remainingBytes = 0;
					return false;
				}
			}
			
			bool toReturn = currentByte & currentMask;
			currentMask <<= 1;
			return toReturn;
		}
		
		virtual ~FileBitProvider() {
			inputFile.close();
		}
};

#endif
