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

#ifndef ITOQUEUE_HPP
#define ITOQUEUE_HPP

#include <list>
#include "ItoCommon.hpp"
#include "ItoG711Sample.hpp"

typedef std::list<ItoG711Sample*> itoSampleList;

class ItoQueue {
	public:
		itoSampleList samples;
		g726_state_t lowerCodec;
		
		ItoQueue() {}
		
		virtual ~ItoQueue() {}
};

#endif
