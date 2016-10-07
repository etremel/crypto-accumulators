/* 
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <pthread.h>

#include <utils/SHA256.h>

static pthread_mutex_t _sha256Mutex = PTHREAD_MUTEX_INITIALIZER;

MySHA256* MySHA256::_instance = NULL;

MySHA256::MySHA256(){
}

MySHA256* MySHA256::getInstance(){
	if(_instance == NULL){
		pthread_mutex_lock(&_sha256Mutex);
		if(_instance == NULL){
			_instance = new MySHA256();
		}
		pthread_mutex_unlock(&_sha256Mutex);
	}
	return _instance;
}

void MySHA256::computeDigest(const char *input, int length, vector<unsigned char> &output){
	unsigned char tempOutput[SHA256_DIGEST_LENGTH];
	
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, input, length);
	SHA256_Final(tempOutput, &sha256);

	output.clear();
	for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
		output.push_back(tempOutput[i]);
	}
}

void MySHA256::computeAccumulatorDigest(const G *acc, vector<unsigned char> &output){
	computeDigest(acc->getByteBuffer(), acc->getSize(), output);
}

bool MySHA256::isHashesEqual(const vector<unsigned char> &hash1, const vector<unsigned char> &hash2){
	if(hash1.size() != hash2.size() || hash1.size() != SHA256_DIGEST_LENGTH){
		return false;
	}

	for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
		if(hash1[i] != hash2[i]){
			return false;
		}
	}	
	
	return true;
}

