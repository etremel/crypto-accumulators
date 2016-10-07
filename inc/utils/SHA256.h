/* 
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _SHA_256_H_
#define _SHA_256_H_

extern "C" {
	#include <openssl/sha.h>
}

#include <vector>
#include <bilinear/G.h>

using std::vector;

class MySHA256{
	public:
		static MySHA256* getInstance();

		void computeDigest(const char *input, int length, vector<unsigned char> &output);
		void computeAccumulatorDigest(const G *acc, vector<unsigned char> &output);
		bool isHashesEqual(const vector<unsigned char> &hash1, const vector<unsigned char> &hash2);
	protected:
		MySHA256();
	private:
		static MySHA256 *_instance;
};

#endif /* _SHA_256_H_ */
