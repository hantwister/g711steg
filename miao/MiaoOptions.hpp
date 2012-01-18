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

#ifndef MIAOOPTIONS_HPP
#define MIAOOPTIONS_HPP

#define KVAR_OPTION "k"
#define KVAR_KEY 'k'
#define LAMBDA_OPTION "maxLambda"
#define LAMBDA_KEY 'l'

#include <argp.h>

error_t miaoParser (int key, char *arg, struct argp_state *state);

static struct argp_option miaoArgp_opts[] = { // options
	{KVAR_OPTION, KVAR_KEY, KVAR_OPTION, 0, "Controls the number of samples processed at a time"},
	{LAMBDA_OPTION, LAMBDA_KEY, LAMBDA_OPTION, 0, "Controls tolerance for extreme values"},
	{ 0 }
};

static struct argp miaoArgp_base = { // parsers
	miaoArgp_opts, // options
	miaoParser, // parsing function
	0, // no non-option arguments
	0 // no extra documentation
};

static struct argp_child miaoArgp[] = {
	{
		&miaoArgp_base, // actual parser
		0, // no flags
		"Steganography by Miao and Huang", // header
		10 // show after main's option groups
	},
	{ 0 }
};

#endif
