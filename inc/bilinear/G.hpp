/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _G_H_
#define _G_H_

#include <fstream>

#include <bilinear/Scalar.hpp>

/*
 * Abstract interface for an object in group G1/G2, any implementation of
 * bilinear pairing library must support APIs defined by this interface
 */

class G {
public:
    // Constructor, the object is initialized as the generator of the group
    G();

    // Destructor
    virtual ~G() = 0;

    // Assignment operator
    virtual G& operator=(const G& other) = 0;

    // Call this function to become a random element of the group
    virtual void generateRandom() = 0;

    // Call this function to become the identity element of the group
    virtual void becomeIdentity() = 0;

    // Call this function to become the generator element of the group
    virtual void becomeGenerator() = 0;

    // Check if this object and other are the same element
    virtual bool isEqual(const G& other) = 0;

    // Multiplication: result = this * other
    virtual void doMultiplication(const G& other, G& result) = 0;

    // Power: result = this ^ scalar
    virtual void doPower(const Scalar& scalar, G& result) = 0;

    // Import the underlying implementation to this object
    virtual void importObject(const void* obj) = 0;

    // Export this object to the underlying implementation
    virtual void exportObject(void* obj) const = 0;

    // Size of this object in byte
    virtual size_t getSize() const = 0;

    // Serialize this object to a read-only byte buffer
    virtual char* getByteBuffer() const = 0;

    // Import object from file
    virtual void readFromFile(std::istream& inFile) = 0;

    // Export object to file
    virtual void writeToFile(std::ostream& outFile) const = 0;
};

#endif /* _G_H_ */
