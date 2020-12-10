/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _GT_H_
#define _GT_H_

#include <fstream>

#include <bilinear/Scalar.hpp>

/*
 * Abstract interface for an object in group GT, any implementation of
 * bilinear pairing library must support APIs defined by this interface
 */

class GT {
public:
    // Constructor
    GT();

    // Destructor
    virtual ~GT() = 0;

    // Assignment operator
    virtual GT& operator=(const GT& e) = 0;

    // Multiplication: result = this * other
    virtual void doMultiplication(const GT& other, GT& result) = 0;

    // Power: result = this ^ scalar
    virtual void doPower(const Scalar& scalar, GT& result) = 0;

    // Check if this object and other are the same element
    virtual int isEqual(const GT& other) = 0;

    // Import the underlying implementation to this object
    virtual void importObject(const void* obj) = 0;

    // Export this object to the underlying implementation
    virtual void exportObject(void* obj) const = 0;

    // Import object from file
    virtual void readFromFile(std::istream& inFile) = 0;

    // Export object to file
    virtual void writeToFile(std::ostream& outFile) const = 0;
};

#endif /* _GT_H_ */
