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

#ifndef ITOOPTIONS_HPP
#define ITOOPTIONS_HPP

#define BITRATE_OPTION "g726bitrate"
#define BITRATE_KEY 'b'

#include <argp.h>

error_t itoParser (int key, char *arg, struct argp_state *state);

static struct argp_option itoArgp_opts[] = { // options
	{BITRATE_OPTION, BITRATE_KEY, BITRATE_OPTION, 0, "Bitrate for the comparison codec"},
	{ 0 }
};

static struct argp itoArgp_base = { // parsers
	itoArgp_opts, // options
	itoParser, // parsing function
	0, // no non-option arguments
	0 // no extra documentation
};

static struct argp_child itoArgp[] = {
	{
		&itoArgp_base, // actual parser
		0, // no flags
		"Steganography by Ito et al", // header
		10 // show after main's option groups
	},
	{ 0 }
};

#endif
