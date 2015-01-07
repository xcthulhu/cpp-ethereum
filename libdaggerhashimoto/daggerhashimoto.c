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
#include "num.h"
#include "daggerhashimoto.h"
#include "SHA3.h"

#define BIG_PRIME "13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006045979"

static parameters defaults = {
  .n = 250000,
  .diff = 16384,  // 2**14
  .cache_size = 25000,
  .epochtime = 1000,
  .k = 2,
  .w = 3,
  .is_serial = 0,
  .accesses = 40,
  // .P needs to be initialized in getter
  // .P_plus_1 needs to be initialized in getter
};

static int defaults_initialized = 0;

parameters get_default_params() {
  if (! defaults_initialized ) {
    defaults.P = read_num(BIG_PRIME);
    defaults_initialized = 1;
  }
  return defaults;
}

/*
 * encode_num() reflects the following python implementation
 * Source: https://github.com/ethereum/wiki/wiki/Dagger-Hashimoto
 *
 * def encode_int(x):
 *     "Encode an integer x as a string of 64 characters using a big-endian scheme"
 *     o = ''
 *     for _ in range(64):
 *         o = chr(x % 256) + o
 *         x //= 256
 *     return o
 *
 * def decode_int(s):
 *     "Unencode an integer x from a string of 64 characters using a big-endian scheme"
 *     x = 0
 *     for c in s:
 *         x *= 256
 *         x += ord(c)
 *     return x

 */

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

/*
   Reference implementation:

   def produce_dag(params, seed):
       P = params["P"]
       picker = init = pow(sha3(seed) + 2, 3, P)
       o = [init]
       for i in range(1, params["n"]):
           x = picker = (picker * init) % P
           for _ in range(params["k"]):
               x ^= o[x % i]
           o.append(pow(x, params["w"], P))
       return o

   NOTE:  While the DAG only contains entries with NUM_BITS,
    Certain values used in its calculation will use as much as
    2*NUM_BITS, so double precision is used in those places.
 */

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
/*
   Reference implementation:

   def quick_calc(params, seed, p):
       from copy import deepcopy
       cache_params = deepcopy(params)
       cache_params["n"] = params["cache_size"]
       cache = produce_dag(cache_params, seed)
       init = pow(sha3(seed) + 2, params["w"], params["P"])
       return quick_calc_cached(cache, params, init, p)
*/

num quick_calc(parameters params, const num seed, const int pos) {
  num cache[params.cache_size];
  params.n = params.cache_size;
  produce_dag(cache, params, seed);
  return quick_calc_cached(cache, params, pos);
}

/*
 * def quick_calc_cached(cache, params, p):
 *     P = params["P"]
 *     if p < len(cache):
 *         return cache[p]
 *     else:
 *         x = pow(cache[0], p + 1, P)
 *         for _ in range(params["k"]):
 *             x ^= quick_calc_cached(cache, params, init, x % p)
 *         return pow(x, params["w"], P)
 */

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

/*
 * def hashimoto(dag, params, header, nonce):
 *     mix = sha3(header+nonce)
 *     i = mix % (params["n"] - params["accesses"])
 *     for p in range(i,i+params["accesses"]+1):
 *         mix = pair(mix, dag[p], params["P"])
 *     return sha3(mix)
 */

num hashimoto(const num * dag, const parameters params, const num header, const num nonce) {
  num mix = num_sha3(num_add(header, nonce));
  const unsigned int i = num_mod_uint(mix, params.n - params.accesses);
  for(unsigned int p = i; p <= i + params.accesses; ++p)
    mix = cantor_mod(mix, dag[p], params.P);
  return num_sha3(mix);
}

/*
 * def quick_hashimoto(seed, params, header, nonce):
 *     from copy import deepcopy
 *     cache_params = deepcopy(params)
 *     cache_params["n"] = params["cache_size"]
 *     cache = produce_dag(cache_params, seed)
 *     return quick_hashimoto_cached(cache, params, header, nonce)
*/

num quick_hashimoto(const num seed, parameters params, const num header, const num nonce) {
  const int original_n = params.n;
  num cache[params.cache_size];
  params.n = params.cache_size;
  produce_dag(cache, params, seed);
  params.n = original_n;
  return quick_hashimoto_cached(cache, params, header, nonce);
}

/*
 * def quick_hashimoto_cached(cache, params, header, nonce):
 *     mix = sha3(header+nonce)
 *     i = mix % (params["n"] - params["accesses"])
 *     for p in range(i,i+params["accesses"]+1):
 *         mix = pair(mix, quick_calc_cached(cache, params, p), params["P"])
 *     return sha3(mix)
 */
num quick_hashimoto_cached(const num * cache, const parameters params, const num header, const num nonce) {
  num mix = num_sha3(num_add(header, nonce));
  const unsigned int i = num_mod_uint(mix, params.n - params.accesses);
  for(unsigned int p = i; p <= i + params.accesses; ++p)
    mix = cantor_mod(mix, quick_calc_cached(cache, params, p), params.P);
  return num_sha3(mix);
}
