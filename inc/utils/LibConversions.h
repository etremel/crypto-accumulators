/*
 * LibConversions.h
 *
 *  Created on: Apr 19, 2013
 *      Author: etremel
 */

#ifndef LIBCONVERSIONS_H_
#define LIBCONVERSIONS_H_

#include <algorithm>

#include <flint/flint.h>

#include <flint/BigInt.h>
#include <flint/BigMod.h>

#include <cryptopp/integer.h>

extern "C" {
    #include <string.h>
    #include <scalar.h>
    #include <openssl/bn.h>
    #include <openssl/crypto.h>
}

namespace LibConversions {

/**
 * Retrieves the global group modulus from DCLXVI as a FLINT integer so
 * that FLINT modular operations can be set up.
 *
 * @param modulus the modulus for G1/G2 in DCLXVI as a flint::BigInt
 */
void getModulus(flint::BigInt& modulus);

void scalarToMpz(const scalar_t scalar, mpz_t mpz);
void mpzToScalar(const mpz_t mpz, scalar_t scalar);

void scalarToString(const scalar_t scalar, char *str);
void stringToScalar(const char *str, scalar_t scalar);

/**
 * Converts a Scalar from DCXLVI to a FLINT BigMod, setting the BigMod's
 * modulus to the global group modulus from DCLXVI.
 *
 * @param scalar the scalar
 * @param target the BigMod that should contain the scalar's value
 */
void scalarToBigMod(const scalar_t& scalar, flint::BigMod &target);
void bigModToScalar(const flint::BigMod& bigmod, scalar_t& target);

/**
 * Converts a Crypto++ Integer to a FLINT BigInt with the same value.
 * @param cryptoInt the Crypto++ integer
 * @param flintInt the FLINT BigInt that will take on cryptoInt's value
 */
void CryptoPPToFlint(const CryptoPP::Integer& cryptoInt, flint::BigInt& flintInt);

/**
 * Converts a FLINT BigInt to an array of bytes representing the big-
 * endian binary representation of that number. The given array must be
 * large enough to hold the number, so it must have a size at least
 * ceil(bigint.bitLength()/8).
 *
 * @param bigint the bigint to convert
 * @param byteArray an array of bytes, assumed to be long enough to
 *         store the bigint's numerical value, into which the result
 *         will be placed.
 */
void bigIntToBytes(const flint::BigInt& bigint, unsigned char* byteArray);

/**
 * Converts an array of bytes representing a big-endian binary representation of
 * a number into a FLINT BigInt containing the same number.
 *
 * @param byteArray an array of bytes representing a number
 * @param length the length of the byte array
 * @param bigint the BigInt object in which the result will be placed
 */
void bytesToBigInt(const unsigned char* byteArray, int length, flint::BigInt& bigint);
/**
 * Converts a string containing a hexadecimal representation of a number
 * into a (big-endian) byte array containing the same number, where each
 * byte represents a pair of hex digits in the string. The given array
 * must be long enough to contain the number, that is, half the length
 * of the string.
 *
 * @param hexString a string of hex digits (in ASCII) representing a number
 * @param byteArray an array of bytes assumed to be long enough to
 *         store the number in the string, into which the result will be
 *         placed.
 */
void hexStringToBytes(const std::string &hexString, unsigned char *byteArray);

}


#endif /* LIBCONVERSIONS_H_ */
