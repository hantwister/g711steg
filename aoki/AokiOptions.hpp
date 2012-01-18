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

#ifndef AOKIOPTIONS_HPP
#define AOKIOPTIONS_HPP

#define VARIANCE_OPTION "j"
#define VARIANCE_KEY 'j'

#include <argp.h>

error_t aokiParser (int key, char *arg, struct argp_state *state);

static struct argp_option aokiArgp_opts[] = { // options
	{VARIANCE_OPTION, VARIANCE_KEY, VARIANCE_OPTION, 0, "Amount by which samples absolute value can be shifted"},
	{ 0 }
};

static struct argp aokiArgp_base = { // parsers
	aokiArgp_opts, // options
	aokiParser, // parsing function
	0, // no non-option arguments
	0 // no extra documentation
};

static struct argp_child aokiArgp[] = {
	{
		&aokiArgp_base, // actual parser
		0, // no flags
		"Steganography by Aoki", // header
		10 // show after main's option groups
	},
	{ 0 }
};

#endif
