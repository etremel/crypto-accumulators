/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <mutex>

#include <utils/SHA256.hpp>


static std::mutex _sha256Mutex;

MySHA256* MySHA256::_instance = NULL;

MySHA256::MySHA256() {
}

MySHA256* MySHA256::getInstance() {
    if(_instance == NULL) {
        std::scoped_lock<std::mutex> lock(_sha256Mutex);
        if(_instance == NULL) {
            _instance = new MySHA256();
        }
    }
    return _instance;
}

void MySHA256::computeDigest(const char* input, int length, std::vector<unsigned char>& output) {
	output.assign(SHA256_DIGEST_LENGTH, 0);

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, length);
    SHA256_Final(output.data(), &sha256);

}

void MySHA256::computeAccumulatorDigest(const G* acc, std::vector<unsigned char>& output) {
    computeDigest(acc->getByteBuffer(), acc->getSize(), output);
}

bool MySHA256::isHashesEqual(const std::vector<unsigned char>& hash1, const std::vector<unsigned char>& hash2) {
    if(hash1.size() != hash2.size() || hash1.size() != SHA256_DIGEST_LENGTH) {
        return false;
    }

    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        if(hash1[i] != hash2[i]) {
            return false;
        }
    }

    return true;
}
