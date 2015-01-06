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

#ifndef NUM_H
#define NUM_H
#include <gmp.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


// In the Intel Nehalem architecture
// "Each core in the architecture has a 128-bit write port and a 128-bit read port to the L1 cache."
// Source: "Cache Organization and Memory Management of the Intel Nehalem Computer Architecture" Rolf (2009)
// URL: http://rolfed.com/nehalem/nehalemPaper.pdf

#define NUM_BITS 512 // 4X the read bandwidth
#include <limits.h>
#define NUM_LIMBS (NUM_BITS / (CHAR_BIT * sizeof(mp_limb_t)))
#define DOUBLE_NUM_LIMBS (2*NUM_LIMBS)

  typedef struct {mp_limb_t _mp_d[NUM_LIMBS];} num;
  typedef struct {mp_limb_t _mp_d[DOUBLE_NUM_LIMBS];} double_num;

  extern const num num_zero;
  extern const double_num double_num_zero;
  extern const num num_one;
  extern const num num_two;
  extern const double_num double_num_one;
  num read_num (const char *);
  double_num read_double_num (const char *);
  int write_num (char *, const num);
  int write_double_num (char *, const double_num);
  num uint_to_num (const unsigned int);
  unsigned int num_to_uint (const num);
  double_num uint_to_double_num (const unsigned int);
  unsigned int double_num_to_uint (const double_num);
  double_num num_to_double_num (const num);
  num double_num_to_num (const double_num);
  num num_shiftr (const num, unsigned int);
  mp_limb_t num_shiftr_in_place(num * , unsigned int);
  num num_shiftl (const num, unsigned int);
  mp_limb_t num_shiftl_in_place(num * , unsigned int);
  int num_cmp(const num, const num);
  int double_num_cmp(const double_num, const double_num);
  int num_is_zero(const num);
  int double_num_is_zero(const double_num);
  num num_add(const num, const num);
  double_num double_num_add(const double_num, const double_num);
  mp_limb_t num_add_in_place(num *, const num);
  void num_xor_in_place(num * , const num);
  double_num num_multiply(const num, const num);
  void num_multiply_to_addr(double_num *, const num, const num);

  num double_num_mod_num(const double_num, const num);
  num num_pow_mod(num, int, const num);

  mp_limb_t num_mod_uint(num, unsigned int);
  int num_is_odd(const num);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //NUM_H
