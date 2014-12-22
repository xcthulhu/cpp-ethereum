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
#include <stdio.h>
#include "num.h"
#include <gmp.h>

int readnum (const char * str, num * n) {
    // No builtin for mpn, have to go through mpz
    mpz_t integ;
    int out, i;
    mpz_init (integ);
    out = gmp_sscanf(str, "%Zu", integ);
    for(i = 0; i < NUM_LIMBS; ++i)
      n->data[i] = i < integ->_mp_size ? integ->_mp_d[i] : 0;
    return out;
}

int writenum (char * str, const num n) {
    return gmp_sprintf(str, "%Nu", n.data, NUM_LIMBS);
}
