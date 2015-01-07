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
#include "num.h"
#include <gmp.h>
#include <string.h>

// TODO: Use templates to automatically make this NUM_LIMBS in length ?
const num num_zero = {{0,0,0,0,0,0,0,0,}};
const double_num double_num_zero = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}};
const num num_one = {{1,0,0,0,0,0,0,0,}};
const num num_two = {{2,0,0,0,0,0,0,0,}};
const double_num double_num_one = {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}};

// Reads a string, representing an unsigned integer, to a num
num read_num (const char * str) {
  // No builtin sscanf for mpn, have to go through mpz
  mpz_t integ;
  num n = num_zero;
  mpz_init(integ);
  gmp_sscanf(str, "%Zu", integ);
  memcpy(&n, integ->_mp_d, integ->_mp_size*sizeof(mp_limb_t));
  return n;
}

// Writes a num to a string (as a extended unsigned integer)
int write_num (char * str, const num n) {
  return gmp_sprintf(str, "%Nu", n._mp_d, NUM_LIMBS);
}

// Reads a string, representing an unsigned integer, to a double_num
double_num read_double_num (const char * str) {
  // No builtin sscanf for mpn, have to go through mpz
  mpz_t integ;
  double_num d = double_num_zero;
  mpz_init(integ);
  gmp_sscanf(str, "%Zu", integ);
  memcpy(&d, integ->_mp_d, integ->_mp_size*sizeof(mp_limb_t));
  return d;
}

// Writes a num to a string (as a extended unsigned integer)
int write_double_num (char * str, const double_num d) {
  return gmp_sprintf(str, "%Nu", d._mp_d, DOUBLE_NUM_LIMBS);
}

// Convert an unsigned int to a num
num uint_to_num (const unsigned int x) {
  num n = num_zero;
  n._mp_d[0] = x;
  return n;
}

// Convert an unsigned int to a double_num
double_num uint_to_double_num (const unsigned int x) {
  double_num n = double_num_zero;
  n._mp_d[0] = x;
  return n;
}

// Convert a num back to an unsigned int, keeping only the least significant bits
unsigned int num_to_uint (const num n) {
  return (unsigned int) n._mp_d[0];
}

// Convert a double_num back to an unsigned int, keeping only the least significant bits
unsigned int double_num_to_uint (const double_num n) {
  return (unsigned int) n._mp_d[0];
}

// Convert a num to a double_num
double_num num_to_double_num (const num n) {
  double_num d = double_num_zero;
  memcpy(&d,&n,NUM_LIMBS*sizeof(mp_limb_t));
  return d;
}

// Convert a double_num back to a num, keeping only the least significant bits
num double_num_to_num (const double_num d) {
  num n;
  memcpy(&n,&d,NUM_LIMBS*sizeof(mp_limb_t));
  return n;
}

// Right bit shift
num num_rshift(const num n, unsigned int count) {
  num out;
  mpn_rshift(out._mp_d, n._mp_d, NUM_LIMBS, count);
  return out;
}

// Right bit shift in place
mp_limb_t num_rshift_in_place(num * n, unsigned int count) {
  return mpn_rshift(n->_mp_d, n->_mp_d, NUM_LIMBS, count);
}

// Left bit shift
num num_lshift(const num n, unsigned int count) {
  num out;
  mpn_lshift(out._mp_d, n._mp_d, NUM_LIMBS, count);
  return out;
}

// Left bit shift in place
mp_limb_t num_lshift_in_place(num * n, unsigned int count) {
  return mpn_lshift(n->_mp_d, n->_mp_d, NUM_LIMBS, count);
}

// add two numbers
num num_add(const num a, const num b) {
  num out;
  mpn_add_n(out._mp_d, a._mp_d, b._mp_d, NUM_LIMBS);
  return out;
}

// double add two numbers
double_num double_num_add(const double_num a, const double_num b) {
  double_num out;
  mpn_add_n(out._mp_d, a._mp_d, b._mp_d, DOUBLE_NUM_LIMBS);
  return out;
}

// add a second number to a first number in place
mp_limb_t num_add_in_place(num * a, const num b) {
  return mpn_add_n(a->_mp_d, a->_mp_d, b._mp_d, NUM_LIMBS);
}

// Compare two nums
int num_cmp(const num a, const num b) {
  return mpn_cmp(a._mp_d, b._mp_d, NUM_LIMBS);
}

// Compare two double_nums
int double_num_cmp(const double_num a, const double_num b) {
  return mpn_cmp(a._mp_d, b._mp_d, DOUBLE_NUM_LIMBS);
}

// Report 1 if a num is zero, 0 otherwise
int num_is_zero(const num x) {
  return !num_cmp(x,num_zero);
}

// Report 1 if a double_num is zero, 0 otherwise
int double_num_is_zero(const double_num x) {
  return !double_num_cmp(x,double_num_zero);
}

// xor two nums
num num_xor(const num a, const num b) {
  num out;
  mpn_xor_n(out._mp_d, a._mp_d, b._mp_d, NUM_LIMBS);
  return out;
}

// xor a num with a (constant) second num in place
void num_xor_in_place(num * a, const num b) {
  mpn_xor_n(a->_mp_d, a->_mp_d, b._mp_d, NUM_LIMBS);
}

void num_multiply_to_addr(double_num * out, const num a, const num b) {
  mpn_mul_n(out->_mp_d, a._mp_d, b._mp_d, NUM_LIMBS);
}

double_num num_multiply(const num a, const num b) {
  double_num out = double_num_zero;
  num_multiply_to_addr(&out, a, b);
  return out;
}

void double_num_mod_num_to_addr(num * dest, const double_num a, const num b) {
  mp_limb_t throwaway[DOUBLE_NUM_LIMBS - NUM_LIMBS + 1];
  mpn_tdiv_qr(throwaway, dest->_mp_d, 0, a._mp_d, DOUBLE_NUM_LIMBS, b._mp_d, NUM_LIMBS);
}

num double_num_mod_num(const double_num a, const num b) {
  // NOTE!  b must be greater than 64**(NUM_LIMBS-1) !
  num residue;
  double_num_mod_num_to_addr(&residue, a, b);
  return residue;
}

mp_limb_t num_mod_uint(num a, unsigned int b) {
  mp_limb_t throwaway[NUM_LIMBS];
  return mpn_divmod_1(throwaway, a._mp_d, NUM_LIMBS, b);
}

int num_is_odd(const num n) {
  return n._mp_d[0] & 1;
}

num num_mult_mod(const num a, const num b, const num m) {
  return double_num_mod_num(num_multiply(a, b), m);
}

num num_pow_mod(num a, int b, const num m)
{
  // NOTE! m must be greater than 64**(NUM_LIMBS-1) !
  double_num temp;
  num r = num_one;
  while (1) {
    if (b & 1) {
      num_multiply_to_addr(&temp, a, r);
      double_num_mod_num_to_addr(&r, temp, m);
    }
    b >>= 1;
    if (b == 0)
      break;
    num_multiply_to_addr(&temp, a, a);
    double_num_mod_num_to_addr(&a, temp, m);
  }
  return r;
}

num cantor_mod(num a, const num b, const num m)
{
  // NOTE! m must be greater than 64**(NUM_LIMBS-1) !
  // NOTE! a must be less than 64**NUM_LIMBS - 1 !
  // ((a+b)*(a+b+1) / 2 + b) % m

  double_num temp1;
  num temp2, temp3;
  mp_limb_t throwaway[DOUBLE_NUM_LIMBS - NUM_LIMBS + 1];
  // Compute (a+b) % m and store in temp2
  temp1._mp_d[NUM_LIMBS] = mpn_add_n(temp1._mp_d, a._mp_d, b._mp_d, NUM_LIMBS);
  mpn_tdiv_qr(throwaway, temp2._mp_d, 0, temp1._mp_d, NUM_LIMBS+1, m._mp_d, NUM_LIMBS);
  // Compute (a+b+1) % m and store in temp3
  num_add_in_place(&a,num_one); // assumes a < (64**NUM_LIMBS)-1
  temp1._mp_d[NUM_LIMBS] = mpn_add_n(temp1._mp_d, a._mp_d, b._mp_d, NUM_LIMBS);
  mpn_tdiv_qr(throwaway, temp3._mp_d, 0, temp1._mp_d, NUM_LIMBS+1, m._mp_d, NUM_LIMBS);
  // Compute (a+b)*(a+b+1)/2 % m and store in temp2
  num_multiply_to_addr(&temp1, temp2, temp3);
  mpn_rshift(temp1._mp_d, temp1._mp_d, DOUBLE_NUM_LIMBS, 1);
  double_num_mod_num_to_addr(&temp2, temp1, m);
  // Compute (a+b)*(a+b+1)/2 + b % m and store in temp2
  temp1._mp_d[NUM_LIMBS] = mpn_add_n(temp1._mp_d, temp2._mp_d, b._mp_d, NUM_LIMBS);
  mpn_tdiv_qr(throwaway, temp2._mp_d, 0, temp1._mp_d, NUM_LIMBS+1, m._mp_d, NUM_LIMBS);
  return temp2;
}
