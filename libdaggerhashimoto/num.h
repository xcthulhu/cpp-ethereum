// -*-c-*-
/*
    This file is part of cpp-ethereum.

    cpp-ethereum is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cpp-ethereum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file num.h
 * @author Matthew Wampler-Doty <matt@w-d.org>
 * @date 2014
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <gmp.h>
#include <stdint.h>

// In the Intel Nehalem architecture
// "Each core in the architecture has a 128-bit write port and a 128-bit read port to the L1 cache."
// Source: "Cache Organization and Memory Managementof the Intel Nehalem Computer Architecture" Rolf (2009)
// URL: http://rolfed.com/nehalem/nehalemPaper.pdf

#define NUM_BITS 512 // 4X the read bandwidth
#define NUM_LIMBS 8 // TODO: Compute from template
typedef struct num {mp_limb_t data[NUM_LIMBS];} num;

int readnum (const char *, num *);
int writenum (char *, const num);

#ifdef __cplusplus
}
#endif
