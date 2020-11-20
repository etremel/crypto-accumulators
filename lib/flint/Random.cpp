/*
 * Random.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: etremel
 */

#include <flint/Random.hpp>

namespace flint {

Random::Random() {
    flint_randinit(randomState);
}

Random::~Random() {
    flint_randclear(randomState);
}

BigInt Random::nextInt(unsigned long numBits) {
    fmpz_t random;
    fmpz_randbits(random, randomState, numBits);
    return BigInt(std::move(random));
}

BigInt Random::nextInt(BigInt& upperBound) {
    fmpz_t random;
    fmpz_randm(random, randomState, upperBound.getUnderlyingObject());
    return BigInt(std::move(random));
}

BigInt Random::nextUnsignedInt(unsigned long numBits) {
    fmpz_t random;
    fmpz_randtest_unsigned(random, randomState, numBits);
    return BigInt(std::move(random));
}

BigMod Random::nextMod(BigInt& modulus) {
    return BigMod(nextInt(modulus), modulus);
}

} /* namespace flint */
