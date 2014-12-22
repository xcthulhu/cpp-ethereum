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
/** @file test.cpp
 * @author Matthew Wampler-Doty <matt@w-d.org>
 * @date 2014
 */

#include <gmp.h>
#include "num.h"

#define BOOST_TEST_MODULE Daggerhashimoto
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <cstring>

BOOST_AUTO_TEST_CASE(num_type_has_desired_number_of_bits)
{
    BOOST_CHECK_EQUAL(CHAR_BIT * sizeof(num), NUM_BITS);
}

BOOST_AUTO_TEST_CASE(num_from_string_and_back)
{
    const char * in = "939079079942597099570608";
    num n;
    char out[155];
    readnum(in, &n);
    writenum(out, n);

BOOST_REQUIRE_MESSAGE(
  ! strcmp(in,out),
  "Input String for readnum : " << in << "\n" <<
  "Differs from output String from writenum : " << out << "\n"
);
}

BOOST_AUTO_TEST_CASE(num_from_string_and_back_big)
{
    const char * in = "1340780792994259709957402499820584612747936582059239337772356144372176403007354697680187429816690342769003185818648605085375388281194656994643364900608";
    num n;
    char out[155];
    readnum(in, &n);
    writenum(out, n);

BOOST_REQUIRE_MESSAGE(
  ! strcmp(in,out),
  "Input String for readnum : " << in << "\n" <<
  "Differs from output String from writenum : " << out << "\n"
);
}
