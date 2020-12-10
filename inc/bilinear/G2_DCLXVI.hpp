/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _G2_DCLXVI_H_
#define _G2_DCLXVI_H_

extern "C" {
#include <twistpoint_fp2.h>
}

#include <bilinear/G.hpp>
#include <bilinear/Scalar_DCLXVI.hpp>

/*
 * This is the implementation of DCLXVI (http://cryptojedi.org/crypto/) for G2.
 * It implements all APIs defined in include/bilinear/G.h, refer to that interface
 * for meanings of APIs.
 */

class G2DCLXVI : public G {
public:
    // APIs defined in include/bilinear/G.h
    G2DCLXVI();
    G2DCLXVI(const G2DCLXVI& other);
    G& operator=(const G& other);
    void generateRandom();
    void becomeIdentity();
    void becomeGenerator();
    bool isEqual(const G& other);
    void doMultiplication(const G& other, G& result);
    void doPower(const Scalar& scalar, G& result);
    void importObject(const void* obj);
    void exportObject(void* obj) const;
    size_t getSize() const;
    char* getByteBuffer() const;
    void readFromFile(std::istream& inFile);
    void writeToFile(std::ostream& outFile) const;

    // Get the pointer to G2 object of the underlying DCLXVI implementation
    twistpoint_fp2_struct_t* getUnderlyingObj();
    const twistpoint_fp2_struct_t* getUnderlyingObj() const;

private:
    // The underlying G1 object
    twistpoint_fp2_t _twistpoint;
    // Required by DCLXVI library after doing mul/pow
    void isReduced(twistpoint_fp2_t twistpoint);
};

#endif /* _G2_DCLXVI_H_ */
