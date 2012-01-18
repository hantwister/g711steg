/*
 * SpanDSP - a series of DSP components for telephony
 *
 * bitstream.c - Bitstream composition and decomposition routines.
 *
 * Written by Steve Underwood <steveu@coppice.org>
 *
 * Copyright (C) 2006 Steve Underwood
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: bitstream.c,v 1.15 2008/11/30 12:45:09 steveu Exp $
 */

#ifdef __cplusplus
// Minor change by Harrison Neal, 2012 Jan 08, same license as original file
extern "C" {
#endif

/*! \file */

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "spandsp/telephony.h"
#include "spandsp/bitstream.h"

#include "spandsp/private/bitstream.h"

void bitstream_put(bitstream_state_t *s, uint8_t **c, uint32_t value, int bits)
{
    value &= ((1 << bits) - 1);
    if (s->residue + bits <= 32)
    {
        s->bitstream |= (value << s->residue);
        s->residue += bits;
    }
    while (s->residue >= 8)
    {
        s->residue -= 8;
        *(*c)++ = (uint8_t) (s->bitstream & 0xFF);
        s->bitstream >>= 8;
    }
}
/*- End of function --------------------------------------------------------*/

void bitstream_put2(bitstream_state_t *s, uint8_t **c, uint32_t value, int bits)
{
    value &= ((1 << bits) - 1);
    if (s->residue + bits <= 32)
    {
        s->bitstream = (s->bitstream << bits) | value;
        s->residue += bits;
    }
    while (s->residue >= 8)
    {
        s->residue -= 8;
        *(*c)++ = (uint8_t) ((s->bitstream >> s->residue) & 0xFF);
    }
}
/*- End of function --------------------------------------------------------*/

uint32_t bitstream_get(bitstream_state_t *s, const uint8_t **c, int bits)
{
    uint32_t x;

    while (s->residue < bits)
    {
        s->bitstream |= (((uint32_t) *(*c)++) << s->residue);
        s->residue += 8;
    }
    s->residue -= bits;
    x = s->bitstream & ((1 << bits) - 1);
    s->bitstream >>= bits;
    return x;
}
/*- End of function --------------------------------------------------------*/

uint32_t bitstream_get2(bitstream_state_t *s, const uint8_t **c, int bits)
{
    uint32_t x;

    while (s->residue < bits)
    {
        s->bitstream = (s->bitstream << 8) | ((uint32_t) *(*c)++);
        s->residue += 8;
    }
    s->residue -= bits;
    x = (s->bitstream >> s->residue) & ((1 << bits) - 1);
    return x;
}
/*- End of function --------------------------------------------------------*/

void bitstream_flush(bitstream_state_t *s, uint8_t **c)
{
    if (s->residue > 0)
    {
        *(*c)++ = (uint8_t) ((s->bitstream << (8 - s->residue)) & 0xFF);
        s->residue = 0;
    }
}
/*- End of function --------------------------------------------------------*/

void bitstream_flush2(bitstream_state_t *s, uint8_t **c)
{
    if (s->residue > 0)
    {
        *(*c)++ = (uint8_t) ((s->bitstream << (8 - s->residue)) & 0xFF);
        s->residue = 0;
    }
}
/*- End of function --------------------------------------------------------*/

bitstream_state_t *bitstream_init(bitstream_state_t *s)
{
    if (s == NULL)
        return NULL;
    s->bitstream = 0;
    s->residue = 0;
    return s;
}
/*- End of function --------------------------------------------------------*/
/*- End of file ------------------------------------------------------------*/

#ifdef __cplusplus
// Minor change by Harrison Neal, 2012 Jan 08, same license as original file
}
#endif
