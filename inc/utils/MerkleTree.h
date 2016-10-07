/* 
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#ifndef _MERKLE_TREE_H_
#define _MERKLE_TREE_H_

#include <vector>
#include <cmath>

#include <iostream>
#include <fstream>

#include <utils/SHA256.h>

using namespace std;

class HashNode{
	public:
		int _offset;
		vector<unsigned char> _hash;
		HashNode(){
			_offset = -1;
			_hash.clear();
		}
		HashNode(int offset, const vector<unsigned char> &hash){
			_offset = offset;
			_hash = hash;
		}
		HashNode& operator=(const HashNode& node){
			if(this == &node){
				return *this;
			} 

			_offset = node._offset;
			_hash = node._hash;
			return *this;
		}
};

class MerkleTree{
	public:
		MerkleTree();
		~MerkleTree();

		void readFullTreeFromFile(const char *fName);
		void readTreeInfoFromFile(const char *fName);
		void writeTreeToFiles(const char *fullTreeFName, const char *treeInfoFName) const;
		
		void constructTree(const vector< vector<unsigned char> > &hashes);
		void getHashes(int index, vector<HashNode> &hashes) const;
		bool checkHashes(const vector<HashNode> &hashes) const;
		void updateHash(int offset, const vector<unsigned char> &digest);

		int getHeight();	
	private:
		int _height;
		vector<unsigned char> _rootHash;
		vector< vector<unsigned char> > _tree;

		void getParentOffset(int offset, int level, int &parentOffset) const;
		void getParentSiblingOffset(int offset, int level, int &parentSiblingOffset) const;
};

#endif /* _MERKLE_TREE_H_ */
