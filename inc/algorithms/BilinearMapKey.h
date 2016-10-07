/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _BILINEAR_MAP_KEY_H_
#define _BILINEAR_MAP_KEY_H_

#include <vector>
#include <memory>

#include <bilinear/G.h>
#include <bilinear/Scalar.h>

using namespace std;

class BilinearMapKey{    
    public:
        //first: a vector of G1 elements. second: a vector of G2 elements.
        typedef pair<vector<unique_ptr<G>>, vector<unique_ptr<G>>> PublicKey;
        BilinearMapKey();
        ~BilinearMapKey();
        Scalar &getSecretKey() const;
        PublicKey& getPublicKey() const;
		void readSkFromFile(const char *fName);
		void writeSkToFile(const char *fName) const;
		void readPkFromFile(const char *fName);
		void writePkToFile(const char *fName) const;
	private:
        unique_ptr<Scalar> _sk;
        shared_ptr<PublicKey> _pk;
};

#endif /* _BILINEAR_MAP_KEY_H_ */
