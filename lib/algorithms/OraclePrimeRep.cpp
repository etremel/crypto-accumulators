/*
 * Edward Tremel - etremel@cs.brown.edu
 * March 2013
 */

#include <random>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <algorithms/OraclePrimeRep.h>

#include <utils/testutils.h>
#include <utils/LibConversions.h>

#include <cryptopp/sha.h>

using std::cout;
using std::endl;
using std::hex;
using std::dec;
using std::showbase;
using std::noshowbase;

using std::uint_fast64_t;
using std::uint16_t;

OraclePrimeRep::OraclePrimeRep() {
    //This implementation requires no state between calls to genRepresentative
}

OraclePrimeRep::~OraclePrimeRep() {
}

//Gets the first 64 bits of a bigint to use as a seed for an RNG that can only use 64-bit seeds
uint_fast64_t first64Bits(const flint::BigInt& flintNum) {
    uint_fast64_t first64bits;
    std::string truncatedBitString = flintNum.toHex().substr(0, 16); //16 hex chars is 64 bits
    std::istringstream truncatedStream(truncatedBitString);
    truncatedStream >> hex >> first64bits;
    return first64bits;
}

void OraclePrimeRep::genRepresentative(const flint::BigInt& element, flint::BigInt& representative) {
    typedef std::linear_congruential_engine<uint_fast64_t, 48271, 0, 2147483647> minst_rand_64;
    //Awkwardly convert the bit length of the element into a byte length, rounding up
    size_t byteLength = element.bitLength()/8;
    if(element.bitLength() % 8 != 0)
        byteLength++;
//    cout << "Getting representative for " << element << endl;
    unsigned char bytesToHash[byteLength + SALT_BYTES];
    //Initializing this array is very important because the element and salt may not exactly fill it
    memset(bytesToHash, 0, byteLength + SALT_BYTES);
    //Convert the element to bytes at the beginning of the byte array
    LibConversions::bigIntToBytes(element, bytesToHash);
    //Generate 16 random bits using the element as a seed
    uint_fast64_t elementTruncated = first64Bits(element);
    minst_rand_64 randEngine(elementTruncated);
    std::independent_bits_engine<minst_rand_64, 16, uint16_t> rand16bits(randEngine);
    uint16_t salt = rand16bits();
//    cout << "  Generated random salt: " << salt << endl;
    //Append them to the byte array
    std::copy(reinterpret_cast<const char*>(&salt), reinterpret_cast<const char*>(&salt) + sizeof(salt), &bytesToHash[byteLength]);
    //Hash the element+salt
//    cout << "  Hashing bytes: ";
//    testutils::print_hex(bytesToHash, byteLength+SALT_BYTES);
    unsigned char hashedBytes[CryptoPP::SHA256::DIGESTSIZE];
    CryptoPP::SHA256().CalculateDigest(hashedBytes, bytesToHash, byteLength+SALT_BYTES);
//    cout << "  Result of hash: ";
//    testutils::print_hex(hashedBytes, CryptoPP::SHA256::DIGESTSIZE);
    flint::BigInt hashedElement;
    LibConversions::bytesToBigInt(hashedBytes, CryptoPP::SHA256::DIGESTSIZE, hashedElement);
    //Add some zeroes in the lower-order bits, then find the next prime
//    cout << "Bitshifting hash " << hex << hashedElement << endl;
    hashedElement<<=PADDING_LENGTH;
//    cout << "  Bitshifted hash: " << hex << hashedElement << dec << endl;
//    cout << "Finding next prime..." << endl;
    representative = hashedElement.nextPrime();
//    cout << "  Next probable prime: " << representative << endl;
}
