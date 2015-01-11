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
/** @file daggerhashimoto.c
 * @author Matthew Wampler-Doty <matt@w-d.org>
 * @date 2014
 */
#include <stdlib.h>
#include <stdio.h>
#include "num.h"
#include "daggerhashimoto.h"
#include "SHA3.h"

#define SAFE_PRIME_512 "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006045979"

static parameters defaults = {
  .n = 250000,
  .diff = 16384,  // 2**14
  .cache_size = 25000,
  .epochtime = 1000,
  .k = 2,
  .w = 3,
  .accesses = 200,
  // .P needs to be initialized in getter
};

parameters get_default_params() {
  static int defaults_initialized = 0;
  if (! defaults_initialized ) {
    defaults_initialized = 1;
    defaults.P = read_num(SAFE_PRIME_512);
  }
  return defaults;
}

void encode_num_in_place(enc_num * e, num n) {
// Write all of the digits as chars to the string
  int i = ENCODED_NUM_BYTES;
  while(i) {
    e->char_array[--i] = num_to_uint(n) % 256;
    num_rshift_in_place(&n, 8);
  }
};

enc_num encode_num(const num n) {
  enc_num e;
  encode_num_in_place(&e,n);
  return e;
};

void produce_dag(num * dag, const parameters params, const num seed) {
  const int w = params.w;
  const num
    P = params.P,
    init = num_pow_mod(num_add(num_sha3(seed), num_two), w, P);
  double_num temp;
  num x, picker = init;
  int i, j;
  dag[0] = init;
  for (i = 1; i < params.n; ++i) {
    num_multiply_to_addr(&temp, picker, init);
    x = picker = double_num_mod_num(temp, P);
    for(j = 0; j < params.k; ++j)
      num_xor_in_place(&x, dag[num_mod_uint(x, i)]);
    dag[i] = num_pow_mod(x, w, P);
  }
}

uint32_t pow_mod(const uint32_t a, int b, const uint32_t p)
{
  uint64_t r = 1, aa = a;
  while (1) {
    if (b & 1)
      r = (r * a) % p;
    b >>= 1;
    if (b == 0)
      break;
    aa = (aa * aa) % p;
  }
  return r;
}

num quick_calc(parameters params, const num seed, const int pos) {
  num cache[params.cache_size];
  params.n = params.cache_size;
  produce_dag(cache, params, seed);
  return quick_calc_cached(cache, params, pos);
}

num quick_calc_cached(const num * cache, const parameters params, const int pos) {
  if (pos < params.cache_size)
    return cache[pos];
  else {
    num x = num_pow_mod(cache[0], pos+1, params.P);
    for(int i = 0 ; i < params.k ; ++i)
      num_xor_in_place(&x, quick_calc_cached(cache, params, num_mod_uint(x, pos)));
    return num_pow_mod(x, params.w, params.P);
  }
}

num hashimoto(const num * dag, const parameters params, const num header, const unsigned int nonce) {
  num mix = num_sha3(num_add(header, uint_to_num(nonce)));
  for(int p = 0; p < params.accesses; ++p)
    num_xor_in_place(
      &mix,
      dag[num_to_uint(mix) % params.n]);
  return num_sha3(mix);
}

num quick_hashimoto(const num seed, parameters params, const num header, const unsigned int nonce) {
  const int original_n = params.n;
  num cache[params.cache_size];
  params.n = params.cache_size;
  produce_dag(cache, params, seed);
  params.n = original_n;
  return quick_hashimoto_cached(cache, params, header, nonce);
}

num quick_hashimoto_cached(const num * cache, const parameters params, const num header, const unsigned int nonce) {
  num mix = num_sha3(num_add(header, uint_to_num(nonce)));
  for(int p = 0; p < params.accesses; ++p)
    num_xor_in_place(
      &mix,
      quick_calc_cached(cache, params, num_to_uint(mix) % params.n));
  return num_sha3(mix);
}
