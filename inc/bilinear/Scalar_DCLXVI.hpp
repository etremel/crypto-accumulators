/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _SCALAR_DCLXVI_H_
#define _SCALAR_DCLXVI_H_

#include <iostream>

extern "C" {
#include <scalar.h>
}

//#include <utils/LibConversions.h>
//#include <LiDIA/bigmod.h>
#include <flint/BigMod.hpp>

#include <bilinear/Scalar.hpp>

/*
 * This is the implementation of DCLXVI (http://cryptojedi.org/crypto/) for Zp.
 * It implements all APIs defined in include/bilinear/Scalar.h, refer to that
 * interface for meanings of APIs.
 */

class ScalarDCLXVI : public Scalar {
public:
    // APIs defined in include/bilinear/Scalar.h
    ScalarDCLXVI();
    // Constructor from LiDIA object; shorthand for importLiDIAObject
    // ScalarDCLXVI(const LiDIA::bigmod& bigmod);
    ScalarDCLXVI(const flint::BigMod& bigmod);
    ScalarDCLXVI(const ScalarDCLXVI& other);
    Scalar& operator=(const Scalar& other);
    // Scalar& operator=(const LiDIA::bigmod& other);
    Scalar& operator=(const flint::BigMod& other);
    void generateRandom();
    int getBit(unsigned int pos) const;
    int getSize() const;
    void importObject(const void* obj);
    void exportObject(void* obj) const;
    void importFlintObject(const flint::BigMod& obj);
    void exportFlintObject(flint::BigMod& obj) const;
    // void importLiDIAObject(const LiDIA::bigmod &obj);
    // void exportLiDIAObject(LiDIA::bigmod &obj) const;
    // void exportLiDIABigint(LiDIA::bigint &obj) const;

    void print() const;
    void printPretty() const;
    void readFromFile(std::ifstream& inFile);
    void writeToFile(std::ofstream& outFile) const;

    // Get the pointer to Zp object of the underlying DCLXVI implementation
    unsigned long long* getUnderlyingObj();
    const unsigned long long* getUnderlyingObj() const;

private:
    // The underlying Zp object
    scalar_t _scalar;
    // Size in bits
    unsigned int _size;
};

#endif /* _SCALAR_DCLXVI_H_ */
