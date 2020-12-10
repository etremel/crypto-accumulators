/*
 * Random.h
 *
 *  Created on: Apr 22, 2013
 *      Author: etremel
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include <flint/BigInt.hpp>
#include <flint/BigMod.hpp>
#include <flint/fmpz.h>

namespace flint {

/**
 * Represents a random number generator for FLINT objects. Not copy-constructable
 * or assignable because the state of a random number generator in FLINT cannot
 * be copied or assigned.
 */
class Random {
public:
    Random();
    ~Random();
    /**
     * Generates and returns a signed integer whose absolute value has the
     * given number of bits
     * @param numBits The desired number of bits
     * @return A BigInt containing the random integer.
     */
    BigInt nextInt(unsigned long numBits);
    /**
     * Generates and returns an integer between 0 and upperBound-1,
     * inclusive.
     * @param upperBound The upper bound on the integer to generate.
     * @return A BigInt containing the random integer.
     */
    BigInt nextInt(BigInt& upperBound);
    /**
     * Generates and returns a positive (unsigned) integer whose value has a
     * number of bits which is random from 0 up to numBits, inclusive.
     * @param numBits The highest number of bits the result should have.
     * @return A BigInt containing the random integer.
     */
    BigInt nextUnsignedInt(unsigned long numBits);
    /**
     * Generates and returns a BigMod containing a random value between 0
     * modulus, with its modulus field set to modulus.
     * @param modulus The desired modulus of the new BigMod
     * @return A BigMod containing a random, valid (less than modulus) value.
     */
    BigMod nextMod(BigInt& modulus);

private:
    flint_rand_t randomState;
};

} /* namespace flint */
#endif /* RANDOM_H_ */
