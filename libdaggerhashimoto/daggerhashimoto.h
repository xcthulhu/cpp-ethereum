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
/** @file daggerhashimoto.h
 * @author Matthew Wampler-Doty <matt@w-d.org>
 * @date 2014
 */

#ifndef DAGGERHASHIMOTO_H
#define DAGGERHASHIMOTO_H
#include "num.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <limits.h>
#define ENCODED_NUM_BYTES (NUM_BITS / CHAR_BIT)
typedef struct {char char_array[ENCODED_NUM_BYTES];} enc_num;
typedef struct {
  int n;            // Size of the dataset
  int diff;         // Difficulty (adjusted during block evaluation)
  int epochtime;    // Length of an epoch in blocks (how often the dataset is updated)
  int k;            // Number of parents of a node
  int w;            // Work factor for proof of work during nonce calculations
  int is_serial;    // Is hashimoto modified to be serial?
  int accesses;     // Number of dataset accesses during hashimoto
  num P;              // Number to modulo everything by
} parameters;


parameters get_default_params();

enc_num encode_num(const num);
void encode_num_in_place(enc_num *, num);
void produce_dag(num *, const parameters, const num);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DAGGERHASHIMOTO_H
