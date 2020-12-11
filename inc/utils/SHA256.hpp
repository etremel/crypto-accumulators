/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _SHA_256_H_
#define _SHA_256_H_

#include <bilinear/G.hpp>
#include <vector>

/**
 * Some utility functions for SHA256 hashes
 */
namespace SHA256 {

//Size of a SHA256 hash is 256 bits = 32 bytes
inline constexpr int DIGEST_LENGTH = 32;

void computeDigest(const char* input, int length, std::vector<unsigned char>& output);
void computeAccumulatorDigest(const G* acc, std::vector<unsigned char>& output);
bool isHashesEqual(const std::vector<unsigned char>& hash1, const std::vector<unsigned char>& hash2);

}  // namespace SHA256

#endif /* _SHA_256_H_ */
