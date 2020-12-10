/*
 * Duy Nguyen - duy@cs.brown.edu
 *         May 18, 2011
 */

#include <bilinear/G2_DCLXVI.hpp>
#include <utils/Pointers.hpp>

extern const scalar_t bn_n;
extern const twistpoint_fp2_t bn_twistgen;

G2DCLXVI::G2DCLXVI() {
    twistpoint_fp2_set(_twistpoint, bn_twistgen);
    fp2e_isreduced(_twistpoint->m_x);
    fp2e_isreduced(_twistpoint->m_y);
}

G2DCLXVI::G2DCLXVI(const G2DCLXVI& other) {
    twistpoint_fp2_t eObj;
    other.exportObject(eObj);
    twistpoint_fp2_set(_twistpoint, eObj);
}

G& G2DCLXVI::operator=(const G& other) {
    if(this == &other) {
        return *this;
    } else {
        G2DCLXVI* pG2 = (G2DCLXVI*)&other;
        twistpoint_fp2_t eObj;
        pG2->exportObject(eObj);
        twistpoint_fp2_set(_twistpoint, eObj);
        return *this;
    }
}

void G2DCLXVI::generateRandom() {
    scalar_t scalar;
    scalar_setrandom(scalar, bn_n);
    twistpoint_fp2_scalarmult_vartime(_twistpoint, _twistpoint, scalar);
    twistpoint_fp2_makeaffine(_twistpoint);
    isReduced(_twistpoint);
}

void G2DCLXVI::becomeIdentity() {
    twistpoint_fp2_setneutral(_twistpoint);
}

void G2DCLXVI::becomeGenerator() {
    twistpoint_fp2_set(_twistpoint, bn_twistgen);
    fp2e_isreduced(_twistpoint->m_x);
    fp2e_isreduced(_twistpoint->m_y);
}

bool G2DCLXVI::isEqual(const G& other) {
    const G2DCLXVI& pG2 = ref_cast<G2DCLXVI>(other);
    twistpoint_fp2_t op;
    pG2.exportObject(op);
    twistpoint_fp2_makeaffine(_twistpoint);
    twistpoint_fp2_makeaffine(op);
    return (fp2e_iseq(_twistpoint->m_x, op->m_x)
            && fp2e_iseq(_twistpoint->m_y, op->m_y)
            && fp2e_iseq(_twistpoint->m_z, op->m_z));
}

void G2DCLXVI::doMultiplication(const G& other, G& result) {
    const G2DCLXVI& pG2 = ref_cast<G2DCLXVI>(other);
    G2DCLXVI& pG2Result = ref_cast<G2DCLXVI>(result);
    twistpoint_fp2_add_vartime(pG2Result.getUnderlyingObj(), _twistpoint, pG2.getUnderlyingObj());
    twistpoint_fp2_makeaffine(pG2Result.getUnderlyingObj());
}

void G2DCLXVI::doPower(const Scalar& scalar, G& result) {
    const ScalarDCLXVI& pScalar = ref_cast<ScalarDCLXVI>(scalar);
    G2DCLXVI& pG2 = ref_cast<G2DCLXVI>(result);
    twistpoint_fp2_scalarmult_vartime(pG2.getUnderlyingObj(), _twistpoint, pScalar.getUnderlyingObj());
    twistpoint_fp2_makeaffine(pG2.getUnderlyingObj());
}

void G2DCLXVI::importObject(const void* obj) {
    twistpoint_fp2_struct_t* op = (twistpoint_fp2_struct_t*)obj;
    twistpoint_fp2_set(_twistpoint, op);
}

void G2DCLXVI::exportObject(void* obj) const {
    twistpoint_fp2_struct_t* rop = (twistpoint_fp2_struct_t*)obj;
    twistpoint_fp2_set(rop, _twistpoint);
}

size_t G2DCLXVI::getSize() const {
    return (24 * (4 * sizeof(mydouble)));
}

char* G2DCLXVI::getByteBuffer() const {
    return (char*)_twistpoint;
}

twistpoint_fp2_struct_t* G2DCLXVI::getUnderlyingObj() {
    return _twistpoint;
}

const twistpoint_fp2_struct_t* G2DCLXVI::getUnderlyingObj() const {
    return _twistpoint;
}

void G2DCLXVI::readFromFile(std::istream& inFile) {
    for(int i = 0; i < 24; i++) {
        inFile.read((char*)&_twistpoint->m_x->v[i], sizeof(mydouble));
        inFile.read((char*)&_twistpoint->m_y->v[i], sizeof(mydouble));
        inFile.read((char*)&_twistpoint->m_z->v[i], sizeof(mydouble));
        inFile.read((char*)&_twistpoint->m_t->v[i], sizeof(mydouble));
    }
}

void G2DCLXVI::writeToFile(std::ostream& outFile) const {
    for(int i = 0; i < 24; i++) {
        outFile.write((char*)&_twistpoint->m_x->v[i], sizeof(mydouble));
        outFile.write((char*)&_twistpoint->m_y->v[i], sizeof(mydouble));
        outFile.write((char*)&_twistpoint->m_z->v[i], sizeof(mydouble));
        outFile.write((char*)&_twistpoint->m_t->v[i], sizeof(mydouble));
    }
}

void G2DCLXVI::isReduced(twistpoint_fp2_t twistpoint) {
    fp2e_isreduced(twistpoint->m_x);
    fp2e_isreduced(twistpoint->m_y);
    fp2e_isreduced(twistpoint->m_z);
    fp2e_isreduced(twistpoint->m_t);
}
