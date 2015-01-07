// -*-c++-*-
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
/** @file SHA3.cpp
 * @author Matthew Wampler-Doty <matt@w-d.org>
 * @date 2014
 */

#include "../libdevcrypto/SHA3.h"
#include "num.h"
#include "daggerhashimoto.h"
#include "SHA3.h"
#include <string>
#include <sstream>

num decode_num(const std::string str)
{
  num out = num_zero;
  for(const unsigned char& c : str) {
    num_lshift_in_place(&out, 8);
    num_add_in_place(&out, uint_to_num(c));
  }
  return out;
}

num num_sha3(const num n) {
  enc_num e;
  encode_num_in_place(&e, n);
  const std::string input = std::string(e.char_array, ENCODED_NUM_BYTES);
  return decode_num(dev::sha3(input, false));
}
