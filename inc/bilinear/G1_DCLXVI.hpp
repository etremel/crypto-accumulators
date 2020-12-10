/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _G1_DCLXVI_H_
#define _G1_DCLXVI_H_

extern "C" {
#include <curvepoint_fp.h>
}

#include <bilinear/G.hpp>
#include <bilinear/Scalar_DCLXVI.hpp>

/*
 * This is the implementation of DCLXVI (http://cryptojedi.org/crypto/) for G1.
 * It implements all APIs defined in include/bilinear/G.h, refer to that interface
 * for meanings of APIs.
 */

class G1DCLXVI : public G {
public:
    // APIs defined in include/bilinear/G.h
    G1DCLXVI();
    G1DCLXVI(const G1DCLXVI& other);
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

    // Get the pointer to G1 object of the underlying DCLXVI implementation
    const curvepoint_fp_struct_t* getUnderlyingObj() const;
    curvepoint_fp_struct_t* getUnderlyingObj();

private:
    // The underlying G1 object
    curvepoint_fp_t _curvepoint;
    // Required by DCLXVI library after doing mul/pow
    void isReduced(curvepoint_fp_t curvepoint);
};

#endif /* _G1_DCLXVI_H_ */
