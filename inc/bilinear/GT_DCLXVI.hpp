/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _GT_DCLXVI_H_
#define _GT_DCLXVI_H_

extern "C" {
#include <fp12e.h>
}

#include <bilinear/GT.hpp>
#include <bilinear/Scalar_DCLXVI.hpp>

/*
 * This is the implementation of DCLXVI (http://cryptojedi.org/crypto/) for GT.
 * It implements all APIs defined in include/bilinear/GT.h, refer to that interface
 * for meanings of APIs.
 */

class GTDCLXVI : public GT {
public:
    // APIs defined in include/bilinear/GT.h
    GTDCLXVI();
    GTDCLXVI(const GTDCLXVI& other);
    GT& operator=(const GT& other);
    void doMultiplication(const GT& other, GT& result);
    void doPower(const Scalar& scalar, GT& result);
    int isEqual(const GT& other);
    void importObject(const void* obj);
    void exportObject(void* obj) const;
    void readFromFile(std::istream& inFile);
    void writeToFile(std::ostream& outFile) const;

private:
    // The underlying GT object
    fp12e_t _fp12e;
};

#endif /* _GT_DCLXVI_H_ */
