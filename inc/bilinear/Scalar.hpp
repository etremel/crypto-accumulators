/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _SCALAR_H_
#define _SCALAR_H_

//#include <LiDIA/bigmod.h>
#include <flint/BigMod.hpp>

#include <fstream>

/*
 * Abstract interface for a big number in Zp, any implementation of
 * bilinear pairing library must support APIs defined by this interface
 */

class Scalar {
public:
    // Constructor
    Scalar();

    // Destructor
    virtual ~Scalar() = 0;

    // Assignment operator
    virtual Scalar& operator=(const Scalar& s) = 0;

    // Assignment from LiDIA object; shorthand for importLiDIAObject
    // virtual Scalar& operator=(const LiDIA::bigmod& other) = 0;

    virtual Scalar& operator=(const flint::BigMod& other) = 0;

    // Generate a random number
    virtual void generateRandom() = 0;

    // Get the bit at position pos
    virtual int getBit(unsigned int pos) const = 0;

    // How many bits used to represent this number
    virtual int getSize() const = 0;

    // Import the underlying implementation to this object
    virtual void importObject(const void* obj) = 0;

    // Export this object to the underlying implementation
    virtual void exportObject(void* obj) const = 0;

    // Import an LiDIA object to this object
    // virtual void importLiDIAObject(const LiDIA::bigmod &obj) = 0;

    // Import a FLINT object
    virtual void importFlintObject(const flint::BigMod& obj) = 0;

    // Export this object to an LiDIA object
    // virtual void exportLiDIAObject(LiDIA::bigmod &obj) const = 0;

    // Export this object to a FLINT object
    virtual void exportFlintObject(flint::BigMod& obj) const = 0;

    // Export this object to a LiDIA bigint,
    // which won't preserve the modulus but is necessary for some LiDIA functions
    // virtual void exportLiDIABigint(LiDIA::bigint &obj) const = 0;

    // Print this number in raw hex data
    virtual void print() const = 0;

    // Print this number in user-friendly format
    virtual void printPretty() const = 0;

    // Import object from file
    virtual void readFromFile(std::ifstream& inFile) = 0;

    // Export object to file
    virtual void writeToFile(std::ofstream& outFile) const = 0;
};

#endif /* _SCALAR_H_ */
