/* 
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <pthread.h>

#include <utils/MerkleTree.h>

MerkleTree::MerkleTree(){
}

MerkleTree::~MerkleTree(){
	_tree.clear();
}

void MerkleTree::readFullTreeFromFile(const char *fName){
	ifstream iStream;
	iStream.open(fName, ios::in | ios::binary);
	
	_tree.clear();
	_rootHash.clear();
	
	char hash[SHA256_DIGEST_LENGTH];
	vector<unsigned char> *node;
	while(true){
		iStream.read(hash, SHA256_DIGEST_LENGTH);
		if(iStream.eof()){
			break;
		}
		node = new vector<unsigned char>(SHA256_DIGEST_LENGTH, 0);
		for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
			node->at(i) = (unsigned char)hash[i];
		}
		_tree.push_back(*node);
	}
	cout<<"Loading Merkle tree done. TreeSize = "<<_tree.size()<<endl;
	iStream.close();
	
	_rootHash = _tree[0];
	_height = int(log2(_tree.size() + 1) - 1);
}

void MerkleTree::readTreeInfoFromFile(const char *fName){
	ifstream iStream;
	iStream.open(fName, ios::in | ios::binary);
	
	_tree.clear();
	_rootHash.clear();
	
	iStream.read((char *)&_height, 4);
	
	char hash[SHA256_DIGEST_LENGTH];
	iStream.read(hash, SHA256_DIGEST_LENGTH);
	for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
		_rootHash.push_back((unsigned char)hash[i]);
	}
	iStream.close();
}

void MerkleTree::writeTreeToFiles(const char *fullTreeFName, const char *treeInfoFName) const{
	// Full tree for server
	ofstream sHashStream;
	sHashStream.open(fullTreeFName, ios::out | ios::binary);
	for(int i=0; i<_tree.size(); i++){
		for(int j=0; j<SHA256_DIGEST_LENGTH; j++){
			unsigned char c = _tree[i][j]; 
			sHashStream.write((char *)&c, 1);
		}
	}
	sHashStream.close();

	// Tree info for client: tree height & root hash
	ofstream cHashStream;
	cHashStream.open(treeInfoFName, ios::out | ios::binary);
	cHashStream.write((char *)&_height, 4);
	for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
		unsigned char c = _rootHash[i]; 
		cHashStream.write((char *)&c, 1);
	}
	cHashStream.close();
}

void MerkleTree::constructTree(const vector< vector<unsigned char> > &hashes){
	int hashSize = hashes.size();
	
	double dheight = log2(hashSize);
	
	_height = int(dheight);
	if(dheight - floor(dheight) > 0.0){
		_height += 1;
	}

	_tree.clear();
	_rootHash.clear();
	
	int treeSize = pow(2, _height + 1) - 1;
	_tree.resize(treeSize);

	//cout<<"HashSize = "<<hashSize<<endl;
	//cout<<"Height = "<<height<<endl;
	//cout<<"Size   = "<<treeSize<<endl;

	int offset, startOffset = int(pow(2, _height) - 1);
	//cout<<"StartOffset = "<<startOffset<<endl;
	
	// Fill the bottom of tree
	for(offset=startOffset; offset<startOffset + hashSize; offset++){
		//cout<<offset<<endl;
		_tree[offset] = hashes[offset - startOffset];
	}
	// There may be other empty nodes in case the number of hashes is not power of 2
	for(; offset<treeSize; offset++){
		//cout<<"Empty node "<<offset<<endl;
		_tree[offset] = *(new vector<unsigned char>(SHA256_DIGEST_LENGTH, 0));
	}

	// Fill higher level hashes from leaves
	int parentOffset;
	char parentHash[2*SHA256_DIGEST_LENGTH];
	for(int level=_height; level>0; level--){
		//cout<<"Filling level "<<level<<endl;
		for(offset=int(pow(2, level) - 1); offset<int(pow(2, level +1) - 2); offset+=2){
			//cout<<"\toffset "<<offset<<endl;
			getParentOffset(offset, level, parentOffset);
			//cout<<"\t\tparentOffset "<<parentOffset<<endl;
			
			for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
				parentHash[i] = char(_tree[offset][i]);
				parentHash[i+SHA256_DIGEST_LENGTH] = char(_tree[offset+1][i]);
			}
			MySHA256::getInstance()->computeDigest(parentHash, 2*SHA256_DIGEST_LENGTH, _tree[parentOffset]);
		}
	}

	_rootHash = _tree[0];
}

void MerkleTree::updateHash(int offset, const vector<unsigned char> &digest){
	int parentOffset;
	char parentHash[2*SHA256_DIGEST_LENGTH];

	offset += int(pow(2, _height) - 1);
	_tree[offset] = digest;
	
	if(offset % 2 == 0){
		offset -= 1;
	}
	
	for(int level=_height; level>0; level--){
		getParentOffset(offset, level, parentOffset);
			
		for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
			parentHash[i] = char(_tree[offset][i]);
			parentHash[i+SHA256_DIGEST_LENGTH] = char(_tree[offset+1][i]);
		}
		MySHA256::getInstance()->computeDigest(parentHash, 2*SHA256_DIGEST_LENGTH, _tree[parentOffset]);
	}

	_rootHash = _tree[0];
}

void MerkleTree::getHashes(int index, vector<HashNode> &hashes) const{
	int startOffset, nodeOffset, siblingOffset, parentSiblingOffset;
	HashNode *hashNode;
   
	startOffset = int(pow(2, _height) - 1);
	nodeOffset = startOffset + index;

	hashes.clear();

	// Node's own hash
	//cout<<"NodeOffset = "<<nodeOffset<<endl;
	hashNode = new HashNode(nodeOffset, _tree[nodeOffset]);
	hashes.push_back(*hashNode);

	// Node sibling's hash
	if(nodeOffset % 2){
		siblingOffset = nodeOffset + 1;
	} else {
		siblingOffset = nodeOffset - 1;
	}
	//cout<<"SiblingOffset = "<<siblingOffset<<endl;
	hashNode = new HashNode(siblingOffset, _tree[siblingOffset]);
	hashes.push_back(*hashNode);

	// Node parent's sibling hash
	for(int level=_height; level>0; level--){
		getParentSiblingOffset(nodeOffset, level, parentSiblingOffset);
		nodeOffset = parentSiblingOffset;
		//cout<<"ParentSiblingOffset = "<<parentSiblingOffset<<endl;
		hashNode = new HashNode(parentSiblingOffset, _tree[parentSiblingOffset]);
		hashes.push_back(*hashNode);
	}	
}

bool MerkleTree::checkHashes(const vector<HashNode> &hashes) const{
	int i=0;
	HashNode *a, *b, parent;
	char hashContent[2*SHA256_DIGEST_LENGTH];
	
	if(_height != hashes.size() - 2){
		return false;
	}

	a = (HashNode *)&hashes[i++];
	b = (HashNode *)&hashes[i++];
	for(int level=_height; level>0; level--){
		if(a->_offset < b->_offset){
			for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
				hashContent[i] = char(a->_hash[i]);
				hashContent[i+SHA256_DIGEST_LENGTH] = char(b->_hash[i]);
			}
		} else {
			for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
				hashContent[i] = char(b->_hash[i]);
				hashContent[i+SHA256_DIGEST_LENGTH] = char(a->_hash[i]);
			}
		}
	
		getParentOffset(a->_offset, level-1, parent._offset);
		MySHA256::getInstance()->computeDigest(hashContent, 2*SHA256_DIGEST_LENGTH, parent._hash);
		
		a = &parent;
		b = (HashNode *)&hashes[i++];
	}
	
	return (MySHA256::getInstance()->isHashesEqual(a->_hash, hashes[hashes.size()-1]._hash) && 
			MySHA256::getInstance()->isHashesEqual(_rootHash, a->_hash));
}

int MerkleTree::getHeight(){
	return _height;
}

void MerkleTree::getParentOffset(int offset, int level, int &parentOffset) const{
	int parentStartOffset = int(pow(2, level) - 1),
		childStartOffset = int(pow(2, level+1) - 1);
	parentOffset = parentStartOffset + (offset - childStartOffset)/2;
}

void MerkleTree::getParentSiblingOffset(int offset, int level, int &parentSiblingOffset) const{
	if(level == 1){
		parentSiblingOffset = 0;
		return;
	}
	int parentOffset;
	getParentOffset(offset, level-1, parentOffset);
	if(parentOffset % 2){
		parentSiblingOffset = parentOffset + 1;
	} else {
		parentSiblingOffset = parentOffset - 1;
	}
}

