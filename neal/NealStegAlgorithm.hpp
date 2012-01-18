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

#ifndef NEALSTEGALGORITHM_HPP
#define NEALSTEGALGORITHM_HPP

#include "../ito/ItoStegAlgorithm.hpp"
#include "NealQueue.hpp"
#include <cstdlib>
#include <cstring>

class NealStegAlgorithm : public ItoStegAlgorithm {
	protected:
		virtual void postToQueue(G711Sample sample, ItoQueue **toQueue) {			
			// Run the overridden method as normal (create the queue if applicable)
			ItoStegAlgorithm::postToQueue(sample, toQueue);
			
			// Increment the sample counter.
			NealQueue *thisQueue = (NealQueue*) *toQueue;
			thisQueue->sampleIndexInPacket++;
			
			// If the sampleIndexInPacket is 160, the codec gets reset
			if (thisQueue->sampleIndexInPacket >= SAMPLES_PER_PACKET) {
				g726_init(&(thisQueue->lowerCodec), g726Bitrate,
					G726_ENCODING_LINEAR, G726_PACKING_NONE);
				thisQueue->sampleIndexInPacket = 0;
			}
		}
		
		virtual ItoQueue* allocQueue() {
			return new NealQueue();
		}
		
		virtual void resetQueue(ItoQueue *queue) {
			ItoStegAlgorithm::resetQueue(queue);
			NealQueue *thisQueue = (NealQueue*) queue;
			thisQueue->sampleIndexInPacket = 0;
		}
		
	public:
		NealStegAlgorithm(unsigned int g726bitrate = 40000) :
			ItoStegAlgorithm(g726bitrate) {}
		
		virtual ~NealStegAlgorithm() {}
};

#endif
