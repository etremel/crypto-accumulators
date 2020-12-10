#ifndef PRIMEREPGENERATOR_H
#define PRIMEREPGENERATOR_H

#include <flint/BigInt.hpp>

/**
 * Interface for any prime representative generator that can be used by
 * RSAAccumulator. Different implementations may require different kinds of
 * state to generate their prime representatives, but all should provide the
 * same interface.
 */
class PrimeRepGenerator {
public:
    PrimeRepGenerator();
    virtual ~PrimeRepGenerator() = 0;
    /**
     * Generates a prime representative for the first argument, placing the
     * result in its second argument
     * @param element the bigint for which a representative should be generated
     * @param representative the bigint that will contain the representative
     *         after running this function
     */
    virtual void genRepresentative(const flint::BigInt& element, flint::BigInt& representative) = 0;
};

#endif  // PRIMEREPGENERATOR_H
