/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <bilinear/Scalar_DCLXVI.hpp>
#include <utils/LibConversions.hpp>

#include <mutex>

using std::cout;
using std::dec;
using std::endl;
using std::hex;
using std::uppercase;

extern const scalar_t bn_n;

ScalarDCLXVI::ScalarDCLXVI() {
    _size = 0;
}

//ScalarDCLXVI::ScalarDCLXVI(const LiDIA::bigmod& bigmod) : ScalarDCLXVI() {
//    importLiDIAObject(bigmod);
//}
ScalarDCLXVI::ScalarDCLXVI(const flint::BigMod& bigmod) : ScalarDCLXVI() {
    importFlintObject(bigmod);
}

ScalarDCLXVI::ScalarDCLXVI(const ScalarDCLXVI& s) {
    scalar_t rop;
    s.exportObject(rop);
    for(int i = 0; i < 4; i++) {
        _scalar[i] = rop[i];
    }
    _size = s.getSize();
}

Scalar& ScalarDCLXVI::operator=(const Scalar& other) {
    if(this == &other) {
        return *this;
    } else {
        ScalarDCLXVI* pScalar = (ScalarDCLXVI*)&other;
        scalar_t rop;
        pScalar->exportObject(rop);
        for(int i = 0; i < 4; i++) {
            _scalar[i] = rop[i];
        }
        _size = pScalar->getSize();
        return *this;
    }
}

//Scalar& ScalarDCLXVI::operator=(const LiDIA::bigmod& other) {
//    importLiDIAObject(other);
//    return *this;
//}

Scalar& ScalarDCLXVI::operator=(const flint::BigMod& other) {
    importFlintObject(other);
    return *this;
}

void ScalarDCLXVI::generateRandom() {
    scalar_setrandom(_scalar, bn_n);
    _size = scalar_scanb(_scalar) + 1;
}

int ScalarDCLXVI::getBit(unsigned int pos) const {
    return scalar_getbit(_scalar, pos);
}

int ScalarDCLXVI::getSize() const {
    return _size;
}

unsigned long long* ScalarDCLXVI::getUnderlyingObj() {
    return _scalar;
}

const unsigned long long* ScalarDCLXVI::getUnderlyingObj() const {
    return _scalar;
}

void ScalarDCLXVI::importObject(const void* obj) {
    unsigned long long* op = (unsigned long long*)obj;
    for(int i = 0; i < 4; i++) {
        _scalar[i] = op[i];
    }
    _size = scalar_scanb(_scalar) + 1;
}

void ScalarDCLXVI::exportObject(void* obj) const {
    unsigned long long* rop = (unsigned long long*)obj;
    for(int i = 0; i < 4; i++) {
        rop[i] = _scalar[i];
    }
}

void ScalarDCLXVI::importFlintObject(const flint::BigMod& obj) {
    LibConversions::bigModToScalar(obj, _scalar);
    _size = scalar_scanb(_scalar) + 1;
}

void ScalarDCLXVI::exportFlintObject(flint::BigMod& obj) const {
    LibConversions::scalarToBigMod(_scalar, obj);
}

//void ScalarDCLXVI::importLiDIAObject(const LiDIA::bigmod &obj){
//	LibConversions::zzToScalar(obj, _scalar);
//	_size = scalar_scanb(_scalar) + 1;
//}
//
//void ScalarDCLXVI::exportLiDIAObject(LiDIA::bigmod &obj) const{
//	LibConversions::scalarToZZ(_scalar, obj);
//}
//
//void ScalarDCLXVI::exportLiDIABigint(LiDIA::bigint &obj) const {
//    LibConversions::scalarToLidiaInt(_scalar, obj);
//}

void ScalarDCLXVI::print() const {
    cout << "Raw hex data: { ";
    for(int i = 0; i < 4; i++) {
        cout << hex << uppercase << "0x" << _scalar[i] << " ";
    }
    cout << "}. Size: " << dec << _size << " bits" << endl;
}

void ScalarDCLXVI::printPretty() const {
    // LiDIA::bigmod zzp;
    // exportLiDIAObject(zzp);
    //Give it a dummy modulus, the exportFlintObject method will set it correctly
    flint::BigMod zzp(1);
    exportFlintObject(zzp);
    cout << dec << "In decimal: " << zzp << ". Size: " << _size << " bits" << endl;
}

void ScalarDCLXVI::readFromFile(std::ifstream& inFile) {
    for(int i = 0; i < 4; i++) {
        inFile.read((char*)&_scalar[i], sizeof(unsigned long long));
    }
    _size = scalar_scanb(_scalar) + 1;
}

void ScalarDCLXVI::writeToFile(std::ofstream& outFile) const {
    for(int i = 0; i < 4; i++) {
        outFile.write((char*)&_scalar[i], sizeof(unsigned long long));
    }
}
