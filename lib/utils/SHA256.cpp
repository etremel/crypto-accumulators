/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */


#include <utils/SHA256.hpp>
#include <cryptopp/sha.h>

namespace SHA256 {

void computeDigest(const char* input, int length, std::vector<unsigned char>& output) {
	output.assign(CryptoPP::SHA256::DIGESTSIZE, 0);
    CryptoPP::SHA256().CalculateDigest(output.data(), (const unsigned char*) input, length);
}

void computeAccumulatorDigest(const G* acc, std::vector<unsigned char>& output) {
    computeDigest(acc->getByteBuffer(), acc->getSize(), output);
}

bool isHashesEqual(const std::vector<unsigned char>& hash1, const std::vector<unsigned char>& hash2) {
    if(hash1.size() != hash2.size() || hash1.size() != DIGEST_LENGTH) {
        return false;
    }

    for(int i = 0; i < DIGEST_LENGTH; i++) {
        if(hash1[i] != hash2[i]) {
            return false;
        }
    }

    return true;
}

}
